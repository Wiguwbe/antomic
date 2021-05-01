/*
   Copyright 2020 Alexandre Pires (c.alexandre.pires@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#pragma once
#include "Core/Base.h"
#include "Core/Log.h"
#include "json/json.h"
#include <sys/types.h>
#ifdef ANTOMIC_PLATFORM_WINDOWS
#include <process.h>
#define GET_PROCESS_ID _getpid
#elif ANTOMIC_PLATFORM_LINUX
#include <unistd.h>
#define GET_PROCESS_ID getpid
#endif

namespace Antomic
{
	using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

	struct ProfileResult
	{
		std::string Name;

		FloatingPointMicroseconds Start;
		std::chrono::microseconds ElapsedTime;
		std::thread::id ThreadID;
		int ProcessID;
		std::string Category;
	};

	struct InstrumentationSession
	{
		std::string Name;
	};

	class Instrumentor
	{
	private:
		std::mutex mMutex;
		InstrumentationSession *mCurrentSession;
		std::string mFilepath;
		nlohmann::json mTraceEvents;

	public:
		Instrumentor()
			: mCurrentSession(nullptr)
		{
		}

		void BeginSession(const std::string &name, const std::string &filepath = "results.json")
		{
			std::lock_guard lock(mMutex);
			mFilepath = filepath;
			if (mCurrentSession)
			{
				// If there is already a current session, then close it before beginning new one.
				// Subsequent profiling output meant for the original session will end up in the
				// newly opened session instead.  That's better than having badly formatted
				// profiling output.
				if (Log::GetLogger()) // Edge case: BeginSession() might be before Log::Init()
				{
					ANTOMIC_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, mCurrentSession->Name);
				}
				InternalEndSession();
			}
			mCurrentSession = new InstrumentationSession({name});
		}

		void EndSession()
		{
			std::lock_guard lock(mMutex);
			InternalEndSession();
		}

		void WriteProfile(const ProfileResult &result)
		{
			std::stringstream tidss;
			tidss << result.ThreadID;
			uint64_t tid = std::stoull(tidss.str());

			nlohmann::json traceEvent;
			traceEvent["cat"] = "function," + result.Category;
			traceEvent["dur"] = result.ElapsedTime.count();
			traceEvent["name"] = result.Name;
			traceEvent["ph"] = "X";
			traceEvent["pid"] = result.ProcessID;
			traceEvent["tid"] = tid;
			traceEvent["ts"] = result.Start.count();

			std::lock_guard lock(mMutex);
			mTraceEvents.push_back(traceEvent);
		}

		static Instrumentor &Get()
		{
			static Instrumentor instance;
			return instance;
		}

	private:
		void WriteTraceDocument()
		{
			nlohmann::json document;

			document["otherData"] = "{ \"version\":\"Antomic v" ANTOMIC_VERSION "\"}"_json;
			document["traceEvents"] = mTraceEvents;

			std::ofstream o(mFilepath);
			o << std::setw(4) << document << std::endl;
			o.flush();
		}

		// Note: you must already own lock on mMutex before
		// calling InternalEndSession()
		void InternalEndSession()
		{
			if (mCurrentSession)
			{
				WriteTraceDocument();
				delete mCurrentSession;
				mCurrentSession = nullptr;
			}
		}
	};

	class InstrumentationTimer
	{
	public:
		InstrumentationTimer(const char *name, const char *category)
			: mName(name), mCategory(category), mStopped(false)
		{
			mStartTimepoint = std::chrono::steady_clock::now();
		}

		~InstrumentationTimer()
		{
			if (!mStopped)
				Stop();
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::steady_clock::now();
			auto highResStart = FloatingPointMicroseconds{mStartTimepoint.time_since_epoch()};
			auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(mStartTimepoint).time_since_epoch();

			Instrumentor::Get().WriteProfile({mName, highResStart, elapsedTime, std::this_thread::get_id(), GET_PROCESS_ID(), mCategory});

			mStopped = true;
		}

	private:
		const char *mName, *mCategory;
		std::chrono::time_point<std::chrono::steady_clock> mStartTimepoint;
		bool mStopped;
	};

	namespace InstrumentorUtils
	{

		template <size_t N>
		struct ChangeResult
		{
			char Data[N];
		};

		template <size_t N, size_t K>
		constexpr auto CleanupOutputString(const char (&expr)[N], const char (&remove)[K])
		{
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N)
			{
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == K - 1)
					srcIndex += matchIndex;
				result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}
			return result;
		}
	} // namespace InstrumentorUtils
} // namespace Antomic

#ifndef ANTOMIC_PROFILE
#define ANTOMIC_PROFILE 0
#endif
#if ANTOMIC_PROFILE
// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define ANTOMIC_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define ANTOMIC_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define ANTOMIC_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define ANTOMIC_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define ANTOMIC_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define ANTOMIC_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define ANTOMIC_FUNC_SIG __func__
#else
#define ANTOMIC_FUNC_SIG "ANTOMIC_FUNC_SIG unknown!"
#endif

#define ANTOMIC_PROFILE_BEGIN_SESSION() ::Antomic::Instrumentor::Get().BeginSession("Antomic Engine")
#define ANTOMIC_PROFILE_END_SESSION() ::Antomic::Instrumentor::Get().EndSession()
#define ANTOMIC_PROFILE_SCOPE(name, category)                                                       \
	constexpr auto fixedName = ::Antomic::InstrumentorUtils::CleanupOutputString(name, "__cdecl "); \
	::Antomic::InstrumentationTimer timer##__LINE__(fixedName.Data,category)
#define ANTOMIC_PROFILE_FUNCTION(category) ANTOMIC_PROFILE_SCOPE(ANTOMIC_FUNC_SIG,category)
#else
#define ANTOMIC_PROFILE_BEGIN_SESSION(name, filepath)
#define ANTOMIC_PROFILE_END_SESSION()
#define ANTOMIC_PROFILE_SCOPE(name)
#define ANTOMIC_PROFILE_FUNCTION()
#endif