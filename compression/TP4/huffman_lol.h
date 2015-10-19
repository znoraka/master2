#pragma once

#include <queue>
#include <vector>
#include <algorithm>
#include <functional>

struct node {
  char c;
  long freq;
  node* left = 0;
  node* right = 0;
};

class Huffman {
 public:
  static char *compress(char* stream, long length) {
    int *freqs = new int[256];
    for (int i = 0; i < 255; i++) {
      freqs[i] = 0;
    }
    for (int i = 0; i < length; i++) {
      freqs[stream[i]]++;
    }

    std::priority_queue<node*, std::vector<node*>, [](node *n1, node *n2) {
      return n1->freq < n2->freq;
    }> q;
  }
};
