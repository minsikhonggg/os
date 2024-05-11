#include "bst_impl.h"

// BST class implementation
BST::BST() : root(nullptr) {
}

BST::~BST() {
}

void BST::insert(int key, int value) {
    Node** cur = &root;
    while (*cur != nullptr) {
        Node* node = *cur;
        if (key < node->key) {
            cur = &node->left;
        } else if (key > node->key) {
            cur = &node->right;
        } else {
            node->value += value;
            node->upd_cnt++;
            return;
        }
    }
    *cur = new Node{key, value, 0, nullptr, nullptr};
}

int BST::lookup(int key) {
    Node* node = root;
    while (node != nullptr) {
        if (key < node->key) {
            node = node->left;
        } else if (key > node->key) {
            node = node->right;
        } else {
            return node->value;
        }
    }
    return 0;  // Key not found
}

void BST::remove(int key) {
    Node** cur = &root;
    Node* node;
    while ((node = *cur) != nullptr) {
        if (key < node->key) {
            cur = &node->left;
        } else if (key > node->key) {
            cur = &node->right;
        } else {
            if (node->left == nullptr || node->right == nullptr) {
                *cur = (node->left != nullptr) ? node->left : node->right;
            } else {
                Node* min = node->right;
                Node** minP = &node->right;
                while (min->left != nullptr) {
                    minP = &min->left;
                    min = min->left;
                }
                node->key = min->key;
                node->value = min->value;
                node->upd_cnt = min->upd_cnt;
                *minP = min->right;
                cur = minP;
                continue;
            }
            delete node;
            break;
        }
    }
}

void BST::traversal(KVC* arr) {
    int index = 0;
    Node* stack[128];  // Assuming a stack size sufficient for most practical cases
    int sp = 0;
    Node* node = root;
    while (node != nullptr || sp > 0) {
        while (node != nullptr) {
            stack[sp++] = node;
            node = node->left;
        }
        node = stack[--sp];
        arr[index].key = node->key;
        arr[index].value = node->value;
        arr[index].upd_cnt = node->upd_cnt;
        index++;
        node = node->right;
    }
}

// CoarseBST class implementation

CoarseBST::CoarseBST() : root(nullptr) {
    pthread_mutex_init(&mutex_lock, nullptr);
}

CoarseBST::~CoarseBST() {
    pthread_mutex_destroy(&mutex_lock);
}

void CoarseBST::insert(int key, int value) {
    pthread_mutex_lock(&mutex_lock);
    Node** cur = &root;
    while (*cur != nullptr) {
        Node* node = *cur;
        if (key < node->key) {
            cur = &node->left;
        } else if (key > node->key) {
            cur = &node->right;
        } else {
            node->value += value;
            node->upd_cnt++;
            pthread_mutex_unlock(&mutex_lock);
            return;
        }
    }
    *cur = new Node{key, value, 0, nullptr, nullptr};
    pthread_mutex_unlock(&mutex_lock);
}

int CoarseBST::lookup(int key) {
    pthread_mutex_lock(&mutex_lock);
    Node* node = root;
    while (node != nullptr) {
        if (key < node->key) {
            node = node->left;
        } else if (key > node->key) {
            node = node->right;
        } else {
            pthread_mutex_unlock(&mutex_lock);
            return node->value;
        }
    }
    pthread_mutex_unlock(&mutex_lock);
    return 0;  // Key not found
}

void CoarseBST::remove(int key) {
    pthread_mutex_lock(&mutex_lock);
    Node** cur = &root;
    Node* node;
    while ((node = *cur) != nullptr) {
        if (key < node->key) {
            cur = &node->left;
        } else if (key > node->key) {
            cur = &node->right;
        } else {
            if (node->left == nullptr || node->right == nullptr) {
                *cur = (node->left != nullptr) ? node->left : node->right;
            } else {
                Node* min = node->right;
                Node** minP = &node->right;
                while (min->left != nullptr) {
                    minP = &min->left;
                    min = min->left;
                }
                node->key = min->key;
                node->value = min->value;
                node->upd_cnt = min->upd_cnt;
                *minP = min->right;
                cur = minP;
                continue;
            }
            delete node;
            break;
        }
    }
    pthread_mutex_unlock(&mutex_lock);
}

void CoarseBST::traversal(KVC* arr) {
    pthread_mutex_lock(&mutex_lock);
    int index = 0;
    Node* stack[128];  // Assuming a stack size sufficient for most practical cases
    int sp = 0;
    Node* node = root;
    while (node != nullptr || sp > 0) {
        while (node != nullptr) {
            stack[sp++] = node;
            node = node->left;
        }
        node = stack[--sp];
        arr[index].key = node->key;
        arr[index].value = node->value;
        arr[index].upd_cnt = node->upd_cnt;
        index++;
        node = node->right;
    }
    pthread_mutex_unlock(&mutex_lock);
}


FineBST::FineBST() : root(nullptr) {}
FineBST::~FineBST() {
    // \ud6c4\uc704 \uc21c\ud68c \ubc29\uc2dd\uc73c\ub85c \ub178\ub4dc\ub97c \uc0ad\uc81c\ud558\uba74\uc11c \uc2a4\ud0dd\uc744 \uc774\uc6a9\ud558\uc5ec \ubc18\ubcf5 \uad6c\ud604
    struct Stack {
        FineNode* node;
        Stack* next;
    };
    Stack* stack = nullptr;
    FineNode* current = root;
    FineNode* last_visited = nullptr;

    while (current != nullptr || stack != nullptr) {
        if (current != nullptr) {
            // \ud604\uc7ac \ub178\ub4dc\ub97c \uc2a4\ud0dd\uc5d0 \uc800\uc7a5\ud558\uace0 \uc67c\ucabd \uc790\uc2dd \ub178\ub4dc\ub85c \uc774\ub3d9
            Stack* new_stack = new Stack{current, stack};
            stack = new_stack;
            current = static_cast<FineNode*>(current->left);
        } else {
            // \uc67c\ucabd \uc790\uc2dd\uc774 \uc5c6\uc73c\uba74 \ud604\uc7ac \ub178\ub4dc\uc758 \uc624\ub978\ucabd \uc790\uc2dd \ub178\ub4dc\ub97c \ud655\uc778
            FineNode* temp = static_cast<FineNode*>(stack->node->right);
            if (temp == nullptr || temp == last_visited) {
                // \uc624\ub978\ucabd \uc790\uc2dd\uc774 \uc5c6\uac70\ub098 \uc774\ubbf8 \ubc29\ubb38\ud55c \ub178\ub4dc\uc778 \uacbd\uc6b0
                // \ud604\uc7ac \ub178\ub4dc\ub97c \uc0ad\uc81c\ud558\uace0 \uc2a4\ud0dd\uc5d0\uc11c \uc81c\uac70
                FineNode* to_delete = stack->node;
                stack = stack->next;
                delete to_delete;
                last_visited = to_delete;
            } else {
                // \uc624\ub978\ucabd \uc790\uc2dd \ub178\ub4dc\ub85c \uc774\ub3d9
                current = temp;
            }
        }
    }
}



void FineBST::insert(int key, int value) {
    FineNode* new_node = new FineNode(key, value, 0, nullptr, nullptr);

    if (root == nullptr) {
        root = new_node;
        return;
    }

    FineNode* cur = root;
    FineNode* parent = nullptr;

    while (true) {
        pthread_mutex_lock(&cur->node_lock);
        parent = cur;

        if (key < cur->key) {
            if (cur->left == nullptr) {
                cur->left = new_node;
                pthread_mutex_unlock(&cur->node_lock);
                return;
            }
            cur = static_cast<FineNode*>(cur->left);
        } else if (key > cur->key) {
            if (cur->right == nullptr) {
                cur->right = new_node;
                pthread_mutex_unlock(&cur->node_lock);
                return;
            }
            cur = static_cast<FineNode*>(cur->right);
        } else {
            cur->value += value;
            cur->upd_cnt++;
            pthread_mutex_unlock(&cur->node_lock);
            delete new_node;
            return;
        }

        pthread_mutex_unlock(&parent->node_lock);
    }
}


int FineBST::lookup(int key) {
    FineNode* cur = root;

    if (cur == nullptr) {
        return 0;
    }

    while (true) {
        pthread_mutex_lock(&cur->node_lock);

        if (key == cur->key) {
            int value = cur->value;
            pthread_mutex_unlock(&cur->node_lock);
            return value;
        } else if (key < cur->key && cur->left != nullptr) {
            FineNode* next = static_cast<FineNode*>(cur->left);
            pthread_mutex_unlock(&cur->node_lock);
            cur = next;
        } else if (key > cur->key && cur->right != nullptr) {
            FineNode* next = static_cast<FineNode*>(cur->right);
            pthread_mutex_unlock(&cur->node_lock);
            cur = next;
        } else {
            pthread_mutex_unlock(&cur->node_lock);
            return 0;
        }
    }
}


void FineBST::remove(int key) {
    FineNode* cur = root;
    FineNode* parent = nullptr;
    FineNode* target = nullptr;
    FineNode* target_parent = nullptr;

    // First, locate the node to be removed
    while (true) {
        pthread_mutex_lock(&cur->node_lock);

        if (key == cur->key) {
            target = cur;
            target_parent = parent;
            pthread_mutex_unlock(&cur->node_lock);
            break;
        } else if (key < cur->key && cur->left != nullptr) {
            FineNode* next = static_cast<FineNode*>(cur->left);
            pthread_mutex_unlock(&cur->node_lock);
            parent = cur;
            cur = next;
        } else if (key > cur->key && cur->right != nullptr) {
            FineNode* next = static_cast<FineNode*>(cur->right);
            pthread_mutex_unlock(&cur->node_lock);
            parent = cur;
            cur = next;
        } else {
            pthread_mutex_unlock(&cur->node_lock);
            return;
        }
    }

    // Now, perform the removal process while holding locks
    pthread_mutex_lock(&target->node_lock);

    if (target->left == nullptr && target->right == nullptr) {
        // Case 1: Node to be removed has no children
        if (target_parent == nullptr) {
            root = nullptr;
        } else if (target_parent->left == target) {
            target_parent->left = nullptr;
        } else {
            target_parent->right = nullptr;
        }
        pthread_mutex_unlock(&target->node_lock);
        delete target;
    } else if (target->left == nullptr || target->right == nullptr) {
        // Case 2: Node to be removed has one child
        FineNode* child = (target->left != nullptr) ? static_cast<FineNode*>(target->left) : static_cast<FineNode*>(target->right);
        if (target_parent == nullptr) {
            root = child;
        } else if (target_parent->left == target) {
            target_parent->left = child;
        } else {
            target_parent->right = child;
        }
        pthread_mutex_unlock(&target->node_lock);
        delete target;
    } else {
        // Case 3: Node to be removed has two children
        FineNode* min = static_cast<FineNode*>(target->right);
        FineNode* min_parent = target;

        while (min->left != nullptr) {
            pthread_mutex_lock(&min->node_lock);
            pthread_mutex_unlock(&min_parent->node_lock);
            min_parent = min;
            min = static_cast<FineNode*>(min->left);
        }

        // Copy the data from the min node to the target node
        target->key = min->key;
        target->value = min->value;
        target->upd_cnt = min->upd_cnt;

        if (min_parent->left == min) {
            min_parent->left = min->right;
        } else {
            min_parent->right = min->right;
        }
        pthread_mutex_unlock(&min_parent->node_lock);
        delete min;
        pthread_mutex_unlock(&target->node_lock);
    }
}



void FineBST::traversal(KVC* arr) {
    FineNode* stack[128];  // Assuming a stack size sufficient for most practical cases
    int sp = 0;
    FineNode* node = root;

    while (node != nullptr || sp > 0) {
        while (node != nullptr) {
            pthread_mutex_lock(&node->node_lock);
            stack[sp++] = node;
            node = static_cast<FineNode*>(node->left);
        }
        node = stack[--sp];
        arr->key = node->key;
        arr->value = node->value;
        arr->upd_cnt = node->upd_cnt;
        arr++;
        node = static_cast<FineNode*>(node->right);
        pthread_mutex_unlock(&stack[sp]->node_lock);
    }
}




