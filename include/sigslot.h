#ifndef SIGSLOT_H
#define SIGSLOT_H

#include <windows.h>
#include <list>
#include <set>

// you can define the default multi-thread policy by define this marco

#ifndef VINH_SIGSLOT_DEFAULT_MT_POLICY
#define VINH_SIGSLOT_DEFAULT_MT_POLICY SingleThread
#endif

namespace vinh
{
	class SingleThread
	{
	public:
		SingleThread()
		{
			;
		}

		virtual ~SingleThread()
		{
			;
		}

		virtual void lock()
		{
			;
		}

		virtual void unlock()
		{
			;
		}
	};

	class MultiThreadGlobal
	{
	public:
		MultiThreadGlobal()
		{
			static bool isInitialized = false;
			if (!isInitialized)
			{
				InitializeCriticalSection(getCritSec());
			}
		}

		virtual ~MultiThreadGlobal()
		{
			;
		}

		virtual void lock()
		{
			EnterCriticalSection(getCritSec());
		}

		virtual void unlock()
		{
			LeaveCriticalSection(getCritSec());
		}

	private:
		static CRITICAL_SECTION* getCritSec()
		{
			static CRITICAL_SECTION critSec;
			return &critSec;
		}
	};

	class MultiThreadLocal
	{
	public:
		MultiThreadLocal()
		{
			InitializeCriticalSection(&mCritSec);
		}

		MultiThreadLocal(const MultiThreadLocal&)
		{
			InitializeCriticalSection(&mCritSec);
		}

		virtual ~MultiThreadLocal()
		{
			DeleteCriticalSection(&mCritSec);
		}

		virtual void lock()
		{
			EnterCriticalSection(&mCritSec);
		}

		virtual void unlock()
		{
			LeaveCriticalSection(&mCritSec);
		}

	private:
		CRITICAL_SECTION mCritSec;
	};

	template <typename MtPolicy>
	class LockBlock
	{
	public:
		LockBlock(MtPolicy* _mutex)
			: mutex(_mutex)
		{
			mutex->lock();
		}

		~LockBlock()
		{
			mutex->unlock();
		}

	private:
		MtPolicy* mutex;
	};

	template <typename MtPolicy>
	class HasSlot;

	template <typename MtPolicy, typename... Args>
	class _ConnectionCore
	{
	public:
		virtual HasSlot<MtPolicy>* getDest() const = 0;
		virtual void emit(Args... args) = 0;
		virtual _ConnectionCore<MtPolicy, Args...>* clone() const = 0;
		virtual _ConnectionCore<MtPolicy, Args...>* duplicate(HasSlot<MtPolicy>* pnewdest) const = 0;
	};

	template <typename MtPolicy>
	class _SignalBaseCore : public MtPolicy
	{
	public:
		virtual void slotDisconnect(HasSlot<MtPolicy>* pslot) = 0;
		virtual void slotDuplicate(const HasSlot<MtPolicy>* poldslot, HasSlot<MtPolicy>* pnewslot) = 0;
	};

	template <typename MtPolicy = VINH_SIGSLOT_DEFAULT_MT_POLICY>
	class HasSlot : public MtPolicy
	{
		using SenderSet = std::set<_SignalBaseCore<MtPolicy>*>;
		using ConstIterator = typename std::set<_SignalBaseCore<MtPolicy>*>::const_iterator;

	public:
		HasSlot()
		{
			;
		}

		HasSlot(const HasSlot<MtPolicy>& other)
			: MtPolicy(other)
		{
			LockBlock<MtPolicy> lock(this);
			ConstIterator it = other.senders.begin();
			ConstIterator itend = other.senders.end();
			while (it != itend)
			{
				(*it)->slotDuplicate(&other, this);
				senders.insert(*it);
				++it;
			}
		}

		virtual ~HasSlot()
		{
			disconnectAll();
		}

		void signalConnect(_SignalBaseCore<MtPolicy>* sender)
		{
			LockBlock<MtPolicy> lock(this);
			senders.insert(sender);
		}

		void signalDisconnect(_SignalBaseCore<MtPolicy>* sender)
		{
			LockBlock<MtPolicy> lock(this);
			senders.erase(sender);
		}

		void disconnectAll()
		{
			LockBlock<MtPolicy> lock(this);
			ConstIterator it = senders.begin();
			ConstIterator itend = senders.end();
			while (it != itend)
			{
				(*it)->slotDisconnect(this);
				++it;
			}
			senders.erase(senders.begin(), senders.end());
		}

	private:
		SenderSet senders;
	};

	template <typename MtPolicy, typename... Args>
	class _SignalBase : public _SignalBaseCore<MtPolicy>
	{
		using ConnectionList = std::list<_ConnectionCore<MtPolicy, Args...>*>;
		using Iterator = typename std::list<_ConnectionCore<MtPolicy, Args...>*>::iterator;
		using ConstIterator = typename std::list<_ConnectionCore<MtPolicy, Args...>*>::const_iterator;

	public:
		_SignalBase()
		{
			;
		}

		_SignalBase(const _SignalBase<MtPolicy, Args...>& other)
			: _SignalBaseCore<MtPolicy>(other)
		{
			LockBlock lock(this);
			ConstIterator it = other.connections.begin();
			ConstIterator itend = other.connections.end();
			while (it != itend)
			{
				(*it)->getDest()->signalConnect(this);
				connections.push_back((*it)->clone());
				++it;
			}
		}

		virtual ~_SignalBase()
		{
			disconnectAll();
		}

		void disconnectAll()
		{
			LockBlock<MtPolicy> lock(this);
			ConstIterator it = connections.begin();
			ConstIterator itend = connections.end();
			while (it != itend)
			{
				(*it)->getDest()->signalDisconnect(this);
				delete* it;
				++it;
			}
			connections.erase(connections.begin(), connections.end());
		}

		void disconnect(HasSlot<MtPolicy>* pclass)
		{
			LockBlock<MtPolicy> lock(this);
			Iterator it = connections.begin();
			Iterator itend = connections.end();
			while (it != itend)
			{
				if ((*it)->getDest() == pclass)
				{
					delete* it;
					connections.erase(it);
					pclass->signalDisconnect(this);
					return;
				}
				++it;
			}
		}

		void slotDisconnect(HasSlot<MtPolicy>* pslot) override
		{
			LockBlock<MtPolicy> lock(this);
			Iterator it = connections.begin();
			Iterator itend = connections.end();
			while (it != itend)
			{
				Iterator next = it;
				++next;
				if ((*it)->getDest() == pslot)
				{
					delete* it;
					connections.erase(it);
				}
				it = next;
			}
		}

		void slotDuplicate(const HasSlot<MtPolicy>* poldslot, HasSlot<MtPolicy>* pnewslot) override
		{
			LockBlock<MtPolicy> lock(this);
			ConstIterator it = connections.begin();
			ConstIterator itend = connections.end();
			while (it != itend)
			{
				if ((*it)->getDest() == poldslot)
				{
					connections.push_back((*it)->duplicate(pnewslot));
				}
				++it;
			}
		}

	protected:
		ConnectionList connections;
	};

	template <typename MtPolicy, typename Dest, typename... Args>
	class _Connection : public _ConnectionCore<MtPolicy, Args...>
	{
	public:
		_Connection()
			: pobject(nullptr), pfunc(nullptr)
		{
			;
		}

		_Connection(Dest* _pobject, void (Dest::* _pfunc)(Args...))
			: pobject(_pobject), pfunc(_pfunc)
		{
			;
		}

		_ConnectionCore<MtPolicy, Args...>* clone() const override
		{
			return new _Connection<MtPolicy, Dest, Args...>(*this);
		}

		_ConnectionCore<MtPolicy, Args...>* duplicate(HasSlot<MtPolicy>* pnewdest) const override
		{
			return new _Connection<MtPolicy, Dest, Args...>(static_cast<Dest*>(pnewdest), pfunc);
		}

		void emit(Args... args) override
		{
			(pobject->*pfunc)(args...);
		}

		HasSlot<MtPolicy>* getDest() const override
		{
			return pobject;
		}

	private:
		Dest* pobject;
		void (Dest::* pfunc)(Args...);
	};

	template <typename MtPolicy, typename... Args>
	class Signal : public _SignalBase<MtPolicy, Args...>
	{
	public:
		Signal()
		{
			;
		}

		Signal(const Signal<MtPolicy, Args...>& other)
			: _SignalBase<MtPolicy, Args...>(other)
		{
			;
		}

		template <typename Dest>
		void connect(Dest* pclass, void (Dest::* pfunc)(Args...))
		{
			LockBlock<MtPolicy> lock(this);
			_Connection<MtPolicy, Dest, Args...>* connection =
				new _Connection<MtPolicy, Dest, Args...>(pclass, pfunc);
			this->connections.push_back(connection);
			pclass->signalConnect(this);
		}

		void emit(Args... args)
		{
			LockBlock<MtPolicy> lock(this);
			auto it = this->connections.begin();
			auto itend = this->connections.end();
			while (it != itend)
			{
				auto next = it;
				++next;
				(*it)->emit(args...);
				it = next;
			}
		}

		void operator()(Args... args)
		{
			LockBlock<MtPolicy> lock(this);
			auto it = this->connections.begin();
			auto itend = this->connections.end();
			while (it != itend)
			{
				auto next = it;
				++next;
				(*it)->emit(args...);
				it = next;
			}
		}
	};

	// Default signal
	template <typename... Args>
	using SIGNAL = Signal<VINH_SIGSLOT_DEFAULT_MT_POLICY, Args...>;

	template <typename... Args>
	using SignalSingle = Signal<SingleThread, Args...>;

	template <typename... Args>
	using SignalGlobal = Signal<MultiThreadGlobal, Args...>;

	template <typename... Args>
	using SignalLocal = Signal<MultiThreadLocal, Args...>;

#define HASSLOT public HasSlot<>
}

#endif
