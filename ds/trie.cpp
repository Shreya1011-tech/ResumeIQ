#include "trie.h"

Trie::Trie() {
    root = new TrieNode();
}

void Trie::insert(string word) {
    TrieNode* node = root;

    for (char c : word) {
        if (node->children.find(c) == node->children.end()) {
            node->children[c] = new TrieNode();
        }
        node = node->children[c];
    }

    node->isEndOfWord = true;
}

bool Trie::search(string word) {
    TrieNode* node = root;

    for (char c : word) {
        if (node->children.find(c) == node->children.end()) {
            return false;
        }
        node = node->children[c];
    }

    return node->isEndOfWord;
}