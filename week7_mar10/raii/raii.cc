// AKA how to avoid goto, tons of if/else, and even try catch

// The hard way
void cv::wait(mutex& m) {
  cpu::interrupt_disable();
  atomic_spin_lock();

  // This thread must hold the lock to wait.
  thread::impl* cur_thread = cpu::self()->impl_ptr->active_thread;
  assert(cur_thread);
  if (m.impl_ptr->owner != cur_thread) {
    atomic_spin_unlock();
    cpu::interrupt_enable();
    throw std::runtime_error("Thread does not hold mutex.");
  }

  // Add thread to cv's queue.
  impl_ptr->wait_queue.push(cur_thread);

  // Release the mutex.
  try {
    internal_unlock_mutex(m);
  } catch (...) {
    // internal_unlock_mutex can throw runtime errors.
    atomic_spin_unlock();
    cpu::interrupt_enable();
    throw;
  }

  // Run the next thread.
  run_thread();

  // Re-acquire the mutex.
  try {
    internal_lock_mutex(m);
  } catch (...) {
    // internal_lock_mutex can throw runtime errors.
    atomic_spin_unlock();
    cpu::interrupt_enable();
    throw;
  }

  internal_interrupt_enable();
  atomic_spin_unlock();
}

// The hard way 2
void cv::wait(mutex& m) {
  cpu::interrupt_disable();
  atomic_spin_lock();

  int err = 0;

  // This thread must hold the lock to wait.
  thread::impl* cur_thread = cpu::self()->impl_ptr->active_thread;
  assert(cur_thread);
  if (m.impl_ptr->owner != cur_thread) {
    err = 1;
    goto fail;
  }

  // Add thread to cv's queue.
  impl_ptr->wait_queue.push(cur_thread);

  // Release the mutex.
  // (internal_unlock_mutex can return != 0 if there is an error)
  if (err = internal_unlock_mutex(m))
    goto fail;
    goto fail;

  // Run the next thread.
  run_thread();

  // Re-acquire the mutex.
  if (err = internal_lock_mutex(m))
    goto fail;

fail:
  atomic_spin_unlock();
  internal_interrupt_enable();

  if (err) {
    throw std::runtime_error("Invalid cv::wait.");
  }
}

// The easy way. Note that CriticalCode won't have any runtime cost over
// the other solutions because compiler magic.

class CriticalCode {
  public:
    CriticalCode();
    ~CriticalCode();
}

CriticalCode::CriticalCode() {
  cpu::disable_interrupts();
  atomic_spin_lock();
}

CriticalCode::~CriticalCode() {
  atomic_spin_unlock();
  cpu::enable_interrupts();
}

void cv::wait(mutex& m) {
  CriticalCode critical;

  // This thread must hold the lock to wait.
  thread::impl* cur_thread = cpu::self()->impl_ptr->active_thread;
  assert(cur_thread);
  if (m.impl_ptr->owner != cur_thread) {
    throw std::runtime_error("Thread does not hold mutex.");
  }

  // Add thread to cv's queue.
  impl_ptr->wait_queue.push(cur_thread);

  // Release the mutex. If it throws, let it.
  internal_unlock_mutex(m);

  // Run the next thread.
  run_thread();

  // Re-acquire the mutex. If it throws, let it.
  internal_lock_mutex(m);
}

// Which of these would you rather write?
// Which of these would you rather read?
// Which of these would you rather debug?
// Which of these would you rather grade?
// Which of these would you rather hire?
//
// :-) Remember -- in CS, laziness is a virtue.
