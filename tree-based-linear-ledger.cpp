#include <bits/stdc++.h>
#include <openssl/sha.h>

using namespace std;

struct TxNode {
    string txId;
    string tableName;
    string operationType;
    string hash; 
    string parentHash;
    vector<shared_ptr<TxNode>> children;

    TxNode(string id, string table, string op)
        : txId(id), tableName(table), operationType(op) {}
};

struct TxListNode {
    string txId;
    string tableName;
    string operationType;
    string hash;
    TxListNode* next;

    TxListNode(string id, string table, string op, string h)
        : txId(id), tableName(table), operationType(op), hash(h), next(nullptr) {}
};

string sha256(const string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return ss.str();
}

void computeHash(const shared_ptr<TxNode>& node) {
    string input = node->parentHash + node->txId + node->tableName + node->operationType;
    node->hash = sha256(input);

    for (auto& child : node->children) {
        child->parentHash = node->hash;
        computeHash(child);
    }
}

void printTree(const shared_ptr<TxNode>& node, int level = 0) {
    if (!node) return;
    cout << string(level * 2, '-')
              << "TxId: " << node->txId
              << ", Table: " << node->tableName
              << ", Op: " << node->operationType
              << ", Hash: " << node->hash << endl;
    for (auto& child : node->children) {
        printTree(child, level + 1);
    }
}

void flattenTreeToList(const shared_ptr<TxNode>& node, TxListNode*& head, TxListNode*& tail) {
    if (!node) return;

    TxListNode* newNode = new TxListNode(node->txId, node->tableName, node->operationType, node->hash);
    if (!head) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }

    for (auto& child : node->children) {
        flattenTreeToList(child, head, tail);
    }
}

void printLinkedList(TxListNode* head) {
    TxListNode* current = head;
    while (current) {
        cout << "TxId: " << current->txId
                  << ", Table: " << current->tableName
                  << ", Op: " << current->operationType
                  << ", Hash: " << current->hash << std::endl;
        current = current->next;
    }
}

int main() {
    auto root = make_shared<TxNode>("rootTx", "Ledger", "CREATE");
    root->parentHash = ""; 

    auto txA1 = make_shared<TxNode>("txA1", "TableA", "INSERT");
    auto txA2 = make_shared<TxNode>("txA2", "TableA", "UPDATE");
    txA1->children.push_back(txA2);
    root->children.push_back(txA1);

    auto txB1 = make_shared<TxNode>("txB1", "TableB", "INSERT");
    auto txB2 = make_shared<TxNode>("txB2", "TableB", "UPDATE");
    txB1->children.push_back(txB2);
    root->children.push_back(txB1);

    auto txC1 = make_shared<TxNode>("txC1", "TableC", "INSERT");
    root->children.push_back(txC1);

    computeHash(root);

    cout << "Tree with Hashes" << endl;
    printTree(root);

    TxListNode* head = nullptr;
    TxListNode* tail = nullptr;
    flattenTreeToList(root, head, tail);

    cout << "\nLinear Ledger Linked List with Hashes" << endl;
    printLinkedList(head);

    while (head) {
        TxListNode* tmp = head;
        head = head->next;
        delete tmp;
    }

    return 0;
}