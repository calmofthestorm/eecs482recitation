#include <cassert>
#include <vector>
#include <iostream>

struct Page {
  size_t id;
  bool ref;
  // ...
};

// Note -- I just used a vector for simplicity to demonstrate the algorithm.
// Think CAREFULLY about which data structures would be most appropriate for an
// eviction queue in real life when you implement P3.
void evict_page(std::vector<Page>& clock_queue, size_t& clock_hand) {
  assert(!clock_queue.empty());
  while (clock_queue[clock_hand].ref) {
    clock_queue[clock_hand].ref = false;
    clock_hand = (clock_hand + 1) % clock_queue.size();
  }
  std::cout << "\nEvicting page " << clock_queue[clock_hand].id << std::endl;
  clock_queue.erase(clock_queue.begin() + clock_hand);
  clock_hand %= clock_queue.size();
}

void print_queue(const std::vector<Page>& clock_queue, size_t clock_hand) {
  std::cout << "Page\tRef" << std::endl;
  for (size_t i = 0; i < clock_queue.size(); ++i) {
    std::cout << clock_queue[i].id << '\t' << clock_queue[i].ref;
    if (i == clock_hand) {
      std::cout << "   <---";
    }
    std::cout << std::endl;
  }
}

int main(int argc, char** argv) {
  std::vector<Page> clock_queue;
  size_t clock_hand;

  std::cin >> clock_hand;
  size_t id;
  bool ref;
  while (std::cin >> id >> ref) {
    clock_queue.push_back(Page{id, ref});
  }

  std::cout << "\nBEFORE\n" << std::endl;
  print_queue(clock_queue, clock_hand);
  evict_page(clock_queue, clock_hand);
  std::cout << "\nAFTER\n" << std::endl;
  print_queue(clock_queue, clock_hand);

  return 0;
}
