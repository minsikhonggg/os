#include "bst_impl.h"

// BST 구현
BST::BST() : root(nullptr) { 
} 

BST::~BST() {
}

void BST::insert(int key, int value) {
    Node* newNode = new Node{key, value, 0, nullptr, nullptr}; // 새로운 노드 생성
    if (root == nullptr) { // 트리가 비어있을 경우
        root = newNode; // 새로운 노드를 루트로 설정
        return;
    }

    Node* current = root; // 현재 노드를 추적

    while (true) {
        if (key < current->key) 
        { // 삽입할 키가 현재 노드의 키보다 작으면
            if (current->left == nullptr) { // 왼쪽 자식이 없는 경우
                current->left = newNode; // 새로운 노드를 왼쪽 자식으로 설정
                return;
            }
            current = current->left; // 왼쪽 자식으로 이동
        } 
        else if (key > current->key) 
        { // 삽입할 키가 현재 노드의 키보다 크면
            if (current->right == nullptr) { // 오른쪽 자식이 없는 경우
                current->right = newNode; // 새로운 노드를 오른쪽 자식으로 설정
                return;
            }
            current = current->right; // 오른쪽 자식으로 이동
        } 
        else 
        { 
            current->value += value; // 동일한 키 발견시 값 업데이트
            current->upd_cnt++; // 업데이트 횟수 증가
            delete newNode; // 새로운 노드 메모리 해제
            return;
        }
    }
}


int BST::lookup(int key) {
    Node* node = root; // 루트에서 시작
    while (node != nullptr) {
        if (key < node->key) {
            node = node->left;
        } else if (key > node->key) {
            node = node->right;
        } else {
            return node->value; // 키가 일치하는 노드 발견시 값을 반환
        }
    }
    return 0;  // 키가 존재하지 않는 경우 0 반환
}

void BST::remove(int key) {
    if (root == nullptr) // 트리가 비어있으면 삭제할 노드가 없음
        return;

    Node* parent = nullptr; // 삭제할 노드의 부모 노드를 추적
    Node* current = root; // 현재 노드를 추적

    while (current != nullptr && current->key != key) {
        parent = current;
        if (key < current->key)
            current = current->left;
        else
            current = current->right;
    }

    if (current == nullptr) // 삭제할 노드가 존재하지 않음
        return;

    // 삭제할 노드를 찾았을 때의 처리
    if (current->left == nullptr || current->right == nullptr) {
        // 자식 노드가 하나도 없거나 하나만 있는 경우
        Node* temp = (current->left != nullptr) ? current->left : current->right;
        if (parent == nullptr)
            root = temp; // 삭제할 노드가 루트일 경우
        else if (parent->left == current)
            parent->left = temp;
        else
            parent->right = temp;
        delete current;
    } else {
        // 자식이 두 개인 경우 오른쪽 서브트리에서 최소값 노드를 찾아 대체
        Node* min = current->right; // 오른쪽 서브트리에서 최소값을 찾기 시작
        Node* minParent = current;
        while (min->left != nullptr) {
            minParent = min;
            min = min->left;
        }
        current->key = min->key; // 최소 노드의 키와 값으로 대체
        current->value = min->value;
        current->upd_cnt = min->upd_cnt;

        if (minParent == current)
            minParent->right = min->right;
        else
            minParent->left = min->right;
        delete min;
    }
}

void BST::traversalRecursive(Node* node, KVC* arr, int& index) {
    if (node == nullptr) {
        return;
    }

    traversalRecursive(node->left, arr, index);
    arr[index].key = node->key;  
    arr[index].value = node->value;
    arr[index].upd_cnt = node->upd_cnt;
    index++; 
    traversalRecursive(node->right, arr, index);
}

void BST::traversal(KVC* arr) {
    int index = 0;
    traversalRecursive(root, arr, index);
}

// CoarseBST 구현
// BST와 동일한 코드 + 전체 lock / 리턴 전 unlock
CoarseBST::CoarseBST() : root(nullptr) {
    pthread_mutex_init(&mutex_lock, nullptr); // 뮤텍스 초기화
}

CoarseBST::~CoarseBST() { 
    pthread_mutex_destroy(&mutex_lock); // 뮤텍스 파괴
}

void CoarseBST::insert(int key, int value) {

    pthread_mutex_lock(&mutex_lock); // 전체 트리 락

    Node* newNode = new Node{key, value, 0, nullptr, nullptr}; // 새로운 노드 생성
    if (root == nullptr) { // 트리가 비어있을 경우
        root = newNode; // 새로운 노드를 루트로 설정
        pthread_mutex_unlock(&mutex_lock); // 락 해제
        return;
    }

    Node* current = root; // 현재 노드를 추적

    while (true) {
        if (key < current->key) { // 삽입할 키가 현재 노드의 키보다 작으면
            if (current->left == nullptr) { // 왼쪽 자식이 없는 경우
                current->left = newNode; // 새로운 노드를 왼쪽 자식으로 설정
                pthread_mutex_unlock(&mutex_lock); // 락 해제
                return;
            }
            current = current->left; // 왼쪽 자식으로 이동
        } else if (key > current->key) { // 삽입할 키가 현재 노드의 키보다 크면
            if (current->right == nullptr) { // 오른쪽 자식이 없는 경우
                current->right = newNode; // 새로운 노드를 오른쪽 자식으로 설정
                pthread_mutex_unlock(&mutex_lock); // 락 해제
                return;
            }
            current = current->right; // 오른쪽 자식으로 이동
        } else { 
            current->value += value; // 동일한 키 발견시 값 업데이트
            current->upd_cnt++; // 업데이트 횟수 증가
            delete newNode; // 새로운 노드 메모리 해제
            pthread_mutex_unlock(&mutex_lock); // 락 해제
            return;
        }
    }
    
}

int CoarseBST::lookup(int key) {

    pthread_mutex_lock(&mutex_lock); // 전체 트리 락

    Node* node = root;
    while (node != nullptr) {
        if (key < node->key) {
            node = node->left;
        } else if (key > node->key) {
            node = node->right;
        } else {
            pthread_mutex_unlock(&mutex_lock); // 락 해제
            return node->value;
        }
    }

    pthread_mutex_unlock(&mutex_lock); // 락 해제
    return 0;  
}

void CoarseBST::remove(int key) {

   pthread_mutex_lock(&mutex_lock); // 전체 트리 락

    if (root == nullptr) { // 트리가 비어있으면 삭제할 노드가 없음
        pthread_mutex_unlock(&mutex_lock); // 락 해제
        return;
    }

    Node* parent = nullptr; // 삭제할 노드의 부모 노드를 추적
    Node* current = root; // 현재 노드를 추적

    while (current != nullptr && current->key != key) {
        parent = current;
        if (key < current->key)
            current = current->left;
        else
            current = current->right;
    }

    if (current == nullptr) { // 삭제할 노드가 존재하지 않음
        pthread_mutex_unlock(&mutex_lock); // 락 해제
        return;
    }

    // 삭제할 노드를 찾았을 때의 처리
    if (current->left == nullptr || current->right == nullptr) {
        // 자식 노드가 하나도 없거나 하나만 있는 경우
        Node* temp = (current->left != nullptr) ? current->left : current->right;
        if (parent == nullptr)
            root = temp; // 삭제할 노드가 루트일 경우
        else if (parent->left == current)
            parent->left = temp;
        else
            parent->right = temp;
        delete current;
    } else {
        // 자식이 두 개인 경우 오른쪽 서브트리에서 최소값 노드를 찾아 대체
        Node* min = current->right; // 오른쪽 서브트리에서 최소값을 찾기 시작
        Node* minParent = current;
        while (min->left != nullptr) {
            minParent = min;
            min = min->left;
        }
        current->key = min->key; // 최소 노드의 키와 값으로 대체
        current->value = min->value;
        current->upd_cnt = min->upd_cnt;

        if (minParent == current)
            minParent->right = min->right;
        else
            minParent->left = min->right;
        delete min;
    }

    pthread_mutex_unlock(&mutex_lock); // 락 해제
}

void CoarseBST::traversalRecursive(Node* node, KVC* arr, int& index) {
    if (node == nullptr) {
        return;
    }

    traversalRecursive(node->left, arr, index);
    arr[index].key = node->key;  
    arr[index].value = node->value;
    arr[index].upd_cnt = node->upd_cnt;
    index++; 
    traversalRecursive(node->right, arr, index);
}

void CoarseBST::traversal(KVC* arr) {
    int index = 0;
    traversalRecursive(root, arr, index);
}

// FineBST 구현
FineBST::FineBST() : root(nullptr) {}
FineBST::~FineBST() {
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
        while (cur->is_being_modified) {
            // remove 작업이 완료될 때까지 대기
            pthread_cond_wait(&cur->remove_cond, &cur->node_lock);
        }
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

            cur->is_being_modified = false;
            pthread_cond_signal(&cur->insert_cond);  // insert 완료 신호
            
            pthread_mutex_unlock(&cur->node_lock);

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

    if (cur == nullptr) {
        return;
    }

    // First, locate the node to be removed
    while (true) {
        pthread_mutex_lock(&cur->node_lock);

        while (cur->is_being_modified) {
            // insert 작업이 완료될 때까지 대기
            pthread_cond_wait(&cur->insert_cond, &cur->node_lock);
        }

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
        
    } else {
        // Case 3: Node to be removed has two children
        FineNode* min = static_cast<FineNode*>(target->right);
        FineNode* min_parent = target;

        while (min != nullptr && min->left != nullptr) {
            pthread_mutex_lock(&min->node_lock);
            pthread_mutex_unlock(&min_parent->node_lock);
            min_parent = min;
            min = static_cast<FineNode*>(min->left);
        }

       if (target != min) {
            target->key = min->key;
            target->value = min->value;
            target->upd_cnt = min->upd_cnt;
        }

        if (min_parent->left == min) {
            min_parent->left = min->right;
        } else {
            min_parent->right = min->right;
        }

        pthread_mutex_unlock(&min_parent->node_lock);

        min->is_being_modified = false;
        pthread_cond_signal(&min->remove_cond);  // remove 완료 신호

        
        pthread_mutex_unlock(&target->node_lock);
    }
}


void FineBST::traversalRecursive(FineNode* node, KVC* arr, int& index) {
    if (node == nullptr) {
        return;
    }

    traversalRecursive(static_cast<FineNode*>(node->left), arr, index);
    arr[index].key = node->key;  
    arr[index].value = node->value;
    arr[index].upd_cnt = node->upd_cnt;
    index++; 
    traversalRecursive(static_cast<FineNode*>(node->right), arr, index);
}

void FineBST::traversal(KVC* arr) {
    int index = 0;
    traversalRecursive(root, arr, index);
}