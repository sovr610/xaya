// Copyright (c) 2018 The Xaya developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_RPC_GAME_H
#define BITCOIN_RPC_GAME_H

#include <sync.h>

#include <memory>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <vector>

class CBlockIndex;

/**
 * The worker for game_sendupdates.  It maintains a queue of work items to
 * process and has a thread that reads the items and performs the work.  It is
 * exposed publicly so that init.cpp can start/interrupt/stop as necessary.
 */
class SendUpdatesWorker
{

public:

  struct Work
  {

    std::string reqtoken;
    std::vector<const CBlockIndex*> detach;
    std::vector<const CBlockIndex*> attach;
    std::set<std::string> trackedGames;

    /* We only allow moving and enforce this by deleted copy constructors.  */
    Work () = default;
    Work (Work&&) = default;
    Work (const Work&) = delete;
    Work& operator= (Work&&) = default;
    void operator= (const Work&) = delete;

    std::string str () const;

  };

private:

  std::queue<Work> work;
  bool interrupted;

  CWaitableCriticalSection csWork;
  CConditionVariable cvWork;

  std::unique_ptr<std::thread> runner;

  static void run (SendUpdatesWorker& self);

public:

  SendUpdatesWorker ();
  ~SendUpdatesWorker ();

  SendUpdatesWorker (const SendUpdatesWorker&) = delete;
  void operator= (const SendUpdatesWorker&) = delete;

  void interrupt ();
  void enqueue (Work&& w);

};

extern std::unique_ptr<SendUpdatesWorker> g_send_updates_worker;

#endif // BITCOIN_RPC_GAME_H
