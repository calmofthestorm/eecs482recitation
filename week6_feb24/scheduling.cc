#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

struct Job {
  std::string name;
  size_t size;
  int priority;
};

std::istream& operator >> (std::istream& in, Job& j) {
  in >> j.name >> j.size >> j.priority;
  return in;
}

struct Event {
  std::string kind;
  std::string name;
  size_t time;
};

std::ostream& operator << (std::ostream& out, const Event& e) {
  out << e.time << "\t" << e.kind << "\t" << e.name;
  return out;
}

std::vector<Event> first_come_first_served(const std::vector<Job>& work) {
  std::vector<Event> events;
  size_t time = 0;
  for (const Job& j : work) {
    events.push_back({"start", j.name, time});
    time += j.size;
    events.push_back({"stop", j.name, time});
  }
  return events;
}

std::vector<Event> round_robin(const std::vector<Job>& work, size_t quantum) {
  assert(quantum > 0);
  size_t time = 0;

  std::vector<Job> todo(work);
  std::vector<Event> events;

  bool done = false;
  while (!done) {
    done = true;
    for (Job& j : todo) {
      if (j.size > 0) {
        done = false;
        events.push_back({"start", j.name, time});
        time += std::min(j.size, quantum);
        j.size -= std::min(j.size, quantum);
        events.push_back({"stop", j.name, time});
      }
    }
  }

  return events;
}

std::vector<Event> shortest_time_to_completion(const std::vector<Job>& work) {
  size_t time = 0;

  std::vector<Job> todo(work);
  std::sort(
      todo.begin(),
      todo.end(),
      [](const Job& a, const Job& b){return a.size < b.size;}
    );
  std::vector<Event> events;

  for (const Job& j : todo) {
    if (j.size > 0) {
      events.push_back({"start", j.name, time});
      time += j.size;
      events.push_back({"stop", j.name, time});
    }
  }

  return events;
}

std::vector<Event> non_preemptive_priority(const std::vector<Job>& work) {
  size_t time = 0;

  std::vector<Job> todo(work);
  std::sort(
      todo.begin(),
      todo.end(),
      [](const Job& a, const Job& b){return a.priority > b.priority;}
    );
  std::vector<Event> events;

  for (const Job& j : todo) {
    if (j.size > 0) {
      events.push_back({"start", j.name, time});
      time += j.size;
      events.push_back({"stop", j.name, time});
    }
  }

  return events;
}

void pretty_print(
    const std::vector<Event>& events,
    const std::vector<Job>& jobs
  ) {

  std::copy(events.begin(), events.end(),
      std::ostream_iterator<Event>(std::cout, "\n")
    );

  // Sigh...ugly...
  std::map<std::string, size_t> response_times;
  for (const Job& j : jobs) {
    response_times[j.name] = j.size;
  }

  for (const Event& ev : events) {
    if (ev.kind == "stop") {
      response_times[ev.name] = ev.time;
    }
  }

  std::cout << "\n\nResponse times:" << std::endl;
  double average_response_time = 0;
  for (const auto& it : response_times) {
    std::cout << it.first << "\t" << it.second << std::endl;
    average_response_time += it.second;
  }
  std::cout << "Average: " << (average_response_time / jobs.size())
            << std::endl;

  std::cout << "\n********************************\n" << std::endl;
}

int main() {
  std::vector<Job> jobs;
  std::copy(
      std::istream_iterator<Job>(std::cin),
      std::istream_iterator<Job>(),
      std::inserter(jobs, jobs.end())
    );

  std::cout << "First come first served:" << std::endl;
  pretty_print(first_come_first_served(jobs), jobs);

  std::cout << "\nRound Robin (quantum 1)" << std::endl;
  pretty_print(round_robin(jobs, 1), jobs);

  std::cout << "\nShortest time to completion" << std::endl;
  pretty_print(shortest_time_to_completion(jobs), jobs);

  std::cout << "\nNon preemptive priority" << std::endl;
  pretty_print(non_preemptive_priority(jobs), jobs);
}
