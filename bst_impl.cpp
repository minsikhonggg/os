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

void BST::traversal(KVC* arr) {
    int index = 0;
    Node* stack[128]; // 스택을 사용한 비재귀 중위 순회
    int sp = 0; // 스택 포인터
    Node* node = root;
    while (node != nullptr || sp > 0) {
        while (node != nullptr) {
            stack[sp++] = node; // 현재 노드를 스택에 푸시하고 왼쪽으로 이동
            node = node->left; 
        }
        node = stack[--sp]; // 스택에서 팝하여 노드 방문
        arr[index].key = node->key;  
        arr[index].value = node->value;
        arr[index].upd_cnt = node->upd_cnt;
        index++; 
        node = node->right; // 오른쪽 자식 노드로 이동
    }
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

void CoarseBST::traversal(KVC* arr) {


    int index = 0;
    Node* stack[128]; // 스택을 사용한 비재귀 중위 순회
    int sp = 0; // 스택 포인터
    Node* node = root;
    while (node != nullptr || sp > 0) {
        while (node != nullptr) {
            stack[sp++] = node; // 현재 노드를 스택에 푸시하고 왼쪽으로 이동
            node = node->left; 
        }
        node = stack[--sp]; // 스택에서 팝하여 노드 방문
        arr[index].key = node->key;  
        arr[index].value = node->value;
        arr[index].upd_cnt = node->upd_cnt;
        index++; 
        node = node->right; // 오른쪽 자식 노드로 이동
    }


}


// FineBST 구현
FineBST::FineBST() : root(nullptr) {}
FineBST::~FineBST() {}


void FineBST::insert(int key, int value) {
            FineNode** node = &root;
            while (*node) {
                FineNode* current = *node;
                lockNode(current);
                if (key == current->key) {
                    current->value += value;
                    current->upd_cnt++;
                    unlockNode(current);
                    return;
                }
                FineNode** next = key < current->key ? &current->left : &current->right;
                if (*next) {
                    unlockNode(current);
                }
                node = next;
            }
            *node = new FineNode(key, value);
            if (*node != root) { // Avoid locking root twice
                unlockNode(*node);
            }
}


int FineBST::lookup(int key) {

            FineNode* node = root;
            while (node) {
                lockNode(node);
                if (key == node->key) {
                    int val = node->value;
                    unlockNode(node);
                    return val;
                }
                FineNode* next = key < node->key ? node->left : node->right;
                unlockNode(node);
                node = next;
            }
            return 0;
}


void FineBST::remove(int key) {


            FineNode** node = &root;
            FineNode* parent = nullptr;

            // Find node to remove and its parent
            while (*node && (*node)->key != key) {
                parent = *node;
                lockNode(parent);
                node = key < parent->key ? &parent->left : &parent->right;
            }
            if (!*node) { // No such key
                if (parent) unlockNode(parent);
                return;
            }

            // Node with two children
            if ((*node)->left && (*node)->right) {
                FineNode* minRight = (*node)->right;
                FineNode* minRightParent = *node;
                lockNode(minRight);

                while (minRight->left) {
                    if (minRightParent != *node) {
                        unlockNode(minRightParent);
                    }
                    minRightParent = minRight;
                    minRight = minRight->left;
                    lockNode(minRight);
                }

                (*node)->key = minRight->key;
                (*node)->value = minRight->value;
                (*node)->upd_cnt = minRight->upd_cnt;
                *node = minRight;
                unlockNode(minRightParent);
            }

            // Node with zero or one child
            FineNode* replacement = (*node)->left ? (*node)->left : (*node)->right;
            delete *node;
            *node = replacement;
            if (parent) unlockNode(parent);
            if (replacement) unlockNode(replacement);
}



void FineBST::traversal(KVC* arr) {

    int index = 0;
    FineNode* stack[128]; // 스택을 사용한 비재귀 중위 순회
    int sp = 0; // 스택 포인터
    FineNode* node = root;
    while (node != nullptr || sp > 0) {
        while (node != nullptr) {
            stack[sp++] = node; // 현재 노드를 스택에 푸시하고 왼쪽으로 이동
            node = static_cast<FineNode*>(node->left); // 왼쪽 자식 노드로 이동한다.
        }
        node = stack[--sp]; // 스택에서 팝하여 노드 방문
        arr[index].key = node->key;  
        arr[index].value = node->value;
        arr[index].upd_cnt = node->upd_cnt;
        index++; 
        node = static_cast<FineNode*>(node->right); // 오른쪽 자식 노드로 이동한다.
    }

}

