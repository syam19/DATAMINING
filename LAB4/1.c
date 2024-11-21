#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 100
#define MAX_TRANSACTIONS 100

// A structure to hold transaction data
typedef struct {
    int items[MAX_ITEMS];
    int num_items;
} Transaction;

// A structure to hold a hashtable entry
typedef struct {
    int itemset[MAX_ITEMS];
    int count;
} HashEntry;

// Hash Table parameters
#define HASH_SIZE 101
HashEntry hashTable[HASH_SIZE];

// Helper function to hash itemsets
int hash(int *itemset, int length) {
    int hashValue = 0;
    for (int i = 0; i < length; i++) {
        hashValue = (hashValue * 31 + itemset[i]) % HASH_SIZE;
    }
    return hashValue;
}

// Hash-Based Algorithm to count itemsets
void hashBasedAlgorithm(Transaction transactions[], int num_transactions, int itemset_size) {
    // Initialize hash table
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].count = 0;
    }
    
    // Count itemsets
    for (int i = 0; i < num_transactions; i++) {
        for (int j = 0; j < transactions[i].num_items - itemset_size + 1; j++) {
            int itemset[itemset_size];
            for (int k = 0; k < itemset_size; k++) {
                itemset[k] = transactions[i].items[j + k];
            }
            int index = hash(itemset, itemset_size);
            hashTable[index].count++;
        }
    }
    
    // Print frequent itemsets
    printf("Hash-Based Algorithm Results:\n");
    for (int i = 0; i < HASH_SIZE; i++) {
        if (hashTable[i].count > 1) {
            printf("Itemset hash %d occurs %d times\n", i, hashTable[i].count);
        }
    }
}

// Partition-Based Algorithm
void partitionBasedAlgorithm(Transaction transactions[], int num_transactions, int itemset_size) {
    printf("Partition-Based Algorithm Results:\n");
    int partitionSize = num_transactions / 2;

    for (int part = 0; part < 2; part++) {
        int start = part * partitionSize;
        int end = (part + 1) * partitionSize;

        int local_counts[MAX_ITEMS] = {0};

        // Count itemsets in the partition
        for (int i = start; i < end; i++) {
            for (int j = 0; j < transactions[i].num_items; j++) {
                local_counts[transactions[i].items[j]]++;
            }
        }

        // Print itemsets for the partition
        printf("Partition %d:\n", part + 1);
        for (int i = 0; i < MAX_ITEMS; i++) {
            if (local_counts[i] > 1) {
                printf("Item %d occurs %d times\n", i, local_counts[i]);
            }
        }
    }
}

// DIC Algorithm (Dynamic Itemset Counting)
void dicAlgorithm(Transaction transactions[], int num_transactions, int min_support) {
    printf("DIC Algorithm Results:\n");
    int counts[MAX_ITEMS] = {0};

    // Scan dataset multiple times dynamically counting itemsets
    for (int pass = 0; pass < 3; pass++) {
        printf("Pass %d:\n", pass + 1);
        for (int i = 0; i < num_transactions; i++) {
            for (int j = 0; j < transactions[i].num_items; j++) {
                counts[transactions[i].items[j]]++;
            }
        }

        // Print counts after each pass
        for (int i = 0; i < MAX_ITEMS; i++) {
            if (counts[i] >= min_support) {
                printf("Item %d has count %d\n", i, counts[i]);
            }
        }
    }
}

// Sample data and main function to execute the algorithms
int main() {
    Transaction transactions[MAX_TRANSACTIONS] = {
        {{1, 2, 3}, 3},
        {{2, 3, 4}, 3},
        {{1, 2, 4}, 3},
        {{1, 3, 4}, 3},
    };
    int num_transactions = 4;
    int itemset_size = 2;
    int min_support = 2;

    hashBasedAlgorithm(transactions, num_transactions, itemset_size);
    partitionBasedAlgorithm(transactions, num_transactions, itemset_size);
    dicAlgorithm(transactions, num_transactions, min_support);

    return 0;
}
