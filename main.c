#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define CAP 4

typedef struct {
  // A simple implementation of a "pair"
  uint16_t first;
  uint16_t second;
} Pair;

typedef struct Node {
  // Node for an alternating tree
  uint16_t treed;
  uint16_t parent;
} Node;

int main() {
  srand (time(NULL));
  int matrx[CAP][CAP];
  for (int k = 0; k < CAP; k++) {
    for (int i = 0; i < CAP; i++) {
      matrx[k][i] = rand() % 200;
    }
  }
  /*
  printf("[");
  for (int k = 0; k < CAP; k++) {
    printf("[");
    for (int i = 0; i < CAP; i++) {
      printf("%d, ", matrx[k][i]);
    }
    printf("]\n");
  }
  printf("]\n");
  */

  // Runs the Hungarian Algorithm on a given matrix and returns the optimal matching with potentials.

  // Step 2: initialize trivial potentials
  // Step 3: initialize alternating tree

  int rpotentials[CAP];
  int cpotentials[CAP];

  uint16_t matching[CAP];
  uint16_t matching_size = 0;
  uint16_t wasMatchedTo, source, matched;
  Node x_nodes[CAP];
  Node y_nodes[CAP];
  // the actual tree
  x_nodes[0].treed = 1;
  x_nodes[0].parent = CAP;
  for (uint16_t i = 1; i < CAP; i++) {
    x_nodes[i].treed = 0;
    x_nodes[i].parent = CAP;
  }
  for (uint16_t i = 0; i < CAP; i++) {
    y_nodes[i].treed = 0;
    y_nodes[i].parent = CAP;
    matching[i] = CAP;
    int* row = matrx[i];
    int maximum = INT_MIN;
    for (uint16_t j = 0; j < CAP; j++) {
      if (row[j] > maximum) maximum = row[j];
    }
    rpotentials[i] = maximum;
    cpotentials[i] = 0;
  }

  // Step 4: Loop while our matching is too small
  while (matching_size != CAP) {
    // Step A: Compute neighbours in equality graph that are not in the tree
    Pair freePair = {CAP, CAP};
    for (uint16_t x = 0; x < CAP; x++) {
      if (x_nodes[x].treed != 0) {
        for (uint16_t y = 0; y < CAP; y++) {
          // check for equality
          if (y_nodes[y].treed == 0) {
            if (matrx[x][y] == rpotentials[x] + cpotentials[y]) {
              // if we found one, break immediately
              freePair.first = x;
              freePair.second = y;
              goto skipPoint;
            }
          }
        }
      }
    }
skipPoint:
    if (freePair.first == CAP) {
      // Step B: If all firms are in the tree, update potentials to get a new one
      int big = INT_MAX;
      // iterate over relevant pairs
      for (uint16_t x = 0; x < CAP; x++) {
        if (x_nodes[x].treed != 0) {
          for (uint16_t y = 0; y < CAP; y++) {
            if (y_nodes[y].treed == 0) {
              // find the difference and check if its smaller than any we found before
              int alpha = rpotentials[x] + cpotentials[y] - matrx[x][y];
              if (alpha < big) {
                big = alpha;
                freePair.first = x;
                freePair.second = y;
              }
            }
          }
        }
      }
      // apply difference to potentials as needed
      for (uint16_t i = 0; i < CAP; i++) {
        if (x_nodes[i].treed != 0) rpotentials[i] -= big;
        if (y_nodes[i].treed != 0) cpotentials[i] += big;
      }
    }

    for (uint16_t i = 0; i < CAP; i++) {
      if (matching[i] == freePair.second) {
        wasMatchedTo = i;
        goto columnNotFree;
      }
    }
    // Step D: Firm is not matched so add it to matching
    matching[freePair.first] = freePair.second;
    // Step E: Swap the alternating path in our alternating tree attached to the worker we matched
    source = freePair.first;
    matched = 1;
    while (1) {
      uint16_t above;
      if (matched) {
        if (x_nodes[source].parent == CAP) goto finished;
        above = x_nodes[source].parent;
      } else {
        above = y_nodes[source].parent;
        matching[above] = source;
      }
      matched = 1 - matched;
      source = above;
    }
finished:
    // Step F: Destroy the tree, go to Step 4 to check completion, and possibly go to Step A
    matching_size++;
    if (matching_size != CAP) {
      // choose a new free row
      for (uint16_t i = 0; i < CAP; i++) {
        if (matching[i] == CAP) {
          for (uint16_t j = 0; j < CAP; j++) {
            x_nodes[j].treed = 0;
            x_nodes[j].parent = CAP;
            y_nodes[j].treed = 0;
            y_nodes[j].parent = CAP;
          }
          x_nodes[i].treed = 1;
          x_nodes[i].parent = CAP;
          break;
        }
      }
    }
    continue;

columnNotFree:
    // Step C: Firm is matched so add it to the tree and go back to Step A
    y_nodes[freePair.second].treed = 1;
    y_nodes[freePair.second].parent = freePair.first;
    x_nodes[wasMatchedTo].treed = 1;
    x_nodes[wasMatchedTo].parent = freePair.second;
  }

  for (int i = 0; i < CAP; i++) {
    printf("(%d, %d), ", i, matching[i]);
  }
  printf("\n");
  for (int i = 0; i < CAP; i++) {
    printf("%d, ", rpotentials[i]);
  }
  printf("\n");
  for (int i = 0; i < CAP; i++) {
    printf("%d, ", cpotentials[i]);
  }
  printf("\n");
}
