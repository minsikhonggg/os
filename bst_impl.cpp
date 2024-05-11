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

    // 새로운 FineNode 생성
    FineNode* new_node = new FineNode(key, value, 0, nullptr, nullptr);

    if (root == nullptr) {
        // 트리가 비어있는 경우 새로운 노드를 루트로 설정한다.
        root = new_node;
        return;
    }

    FineNode* cur = root; // 현재 노드를 추적하기 위한 포인터
    FineNode* parent = nullptr; // 현재 노드의 부모 노드를 추적하기 위한 포인터

    while (true) {
        pthread_mutex_lock(&cur->node_lock); // 현재 노드에 락을 건다.
        parent = cur; // 현재 노드를 부모로 설정한다.

        if (key < cur->key) {
            // 삽입할 키가 현재 노드의 키보다 작은 경우
            if (cur->left == nullptr) {
                // 현재 노드의 왼쪽 자식이 없는 경우 새로운 노드를 왼쪽 자식으로 설정하고 락을 해제한다.
                cur->left = new_node;
                pthread_mutex_unlock(&cur->node_lock); // 락 해제
                return;
            }
            cur = static_cast<FineNode*>(cur->left); // 왼쪽 자식으로 이동한다.
        } else if (key > cur->key) {
            // 삽입할 키가 현재 노드의 키보다 큰 경우
            if (cur->right == nullptr) {
                // 현재 노드의 오른쪽 자식이 없는 경우 새로운 노드를 오른쪽 자식으로 설정하고 락을 해제한다.
                cur->right = new_node;
                pthread_mutex_unlock(&cur->node_lock); // 락 해제
                return;
            }
            cur = static_cast<FineNode*>(cur->right); // 오른쪽 자식으로 이동한다.
        } else {
            // 동일한 키를 갖는 노드가 이미 존재하는 경우
            cur->value += value; // 값 업데이트
            cur->upd_cnt++; // 업데이트 횟수 증가
            delete new_node; // 새로운 노드 삭제
            pthread_mutex_unlock(&cur->node_lock); // 락 해제
            return;
        }

        pthread_mutex_unlock(&parent->node_lock); // 부모 노드의 락 해제
    }
}


int FineBST::lookup(int key) {

    FineNode* cur = root; // 현재 노드를 추적하기 위한 포인터

    if (cur == nullptr) {
        // 트리가 비어있는 경우 0을 반환한다.
        return 0;
    }

    while (true) {
        pthread_mutex_lock(&cur->node_lock); // 현재 노드에 락을 건다.

        if (key == cur->key) {
            // 주어진 키와 현재 노드의 키가 같은 경우 현재 노드의 값을 반환한다.
            int value = cur->value;
            pthread_mutex_unlock(&cur->node_lock); // 락 해제
            return value;
        } else if (key < cur->key && cur->left != nullptr) {
            // 주어진 키가 현재 노드의 키보다 작고, 왼쪽 자식이 있는 경우, 왼쪽 자식으로 이동한다.
            FineNode* next = static_cast<FineNode*>(cur->left);
            pthread_mutex_unlock(&cur->node_lock); // 락 해제
            cur = next; // 현재 노드를 다음 노드로 갱신한다.
        } else if (key > cur->key && cur->right != nullptr) {
            // 주어진 키가 현재 노드의 키보다 크고, 오른쪽 자식이 있는 경우, 오른쪽 자식으로 이동한다.
            FineNode* next = static_cast<FineNode*>(cur->right); 
            pthread_mutex_unlock(&cur->node_lock); // 락 해제
            cur = next; // 현재 노드를 다음 노드로 갱신한다.
        } else {
            // 더 이상 탐색할 수 없는 경우 0을 반환한다.
            pthread_mutex_unlock(&cur->node_lock); // 락 해제
            return 0;
        }
    }
}

void FineBST::remove(int key) {
    // FineBST에서 주어진 키를 갖는 노드를 삭제하는 함수

    FineNode* cur = root; // 현재 노드를 추적하기 위한 포인터
    FineNode* parent = nullptr; // 현재 노드의 부모를 추적하기 위한 포인터
    FineNode* target = nullptr; // 삭제할 노드를 추적하기 위한 포인터
    FineNode* target_parent = nullptr; // 삭제할 노드의 부모를 추적하기 위한 포인터

    // 삭제할 노드를 찾는 과정
    while (true) {
        pthread_mutex_lock(&cur->node_lock); // 현재 노드에 락을 건다.

        if (key == cur->key) {
            // 주어진 키와 현재 노드의 키가 같은 경우 삭제 대상 노드를 찾았으므로 포인터를 설정하고 반복문을 종료한다.
            target = cur;
            target_parent = parent;
            pthread_mutex_unlock(&cur->node_lock); // 락 해제
            break;
        } else if (key < cur->key && cur->left != nullptr) {
            // 주어진 키가 현재 노드의 키보다 작고, 왼쪽 자식이 있는 경우, 왼쪽 자식으로 이동한다.
            FineNode* next = static_cast<FineNode*>(cur->left);
            pthread_mutex_unlock(&cur->node_lock); // 락 해제
            parent = cur; // 부모 노드 포인터를 갱신한다.
            cur = next; // 현재 노드 포인터를 갱신한다.
        } else if (key > cur->key && cur->right != nullptr) {
            // 주어진 키가 현재 노드의 키보다 크고, 오른쪽 자식이 있는 경우, 오른쪽 자식으로 이동한다.
            FineNode* next = static_cast<FineNode*>(cur->right);
            pthread_mutex_unlock(&cur->node_lock); // 락 해제
            parent = cur; // 부모 노드 포인터를 갱신한다.
            cur = next; // 현재 노드 포인터를 갱신한다.
        } else {
            // 더 이상 탐색할 수 없는 경우 반복문을 종료한다.
            pthread_mutex_unlock(&cur->node_lock); // 락 해제
            return;
        }
    }

    // 삭제 과정 실행
    pthread_mutex_lock(&target->node_lock); // 삭제 대상 노드에 락을 건다.

    if (target->left == nullptr && target->right == nullptr) {
        // Case 1: 삭제 대상 노드가 자식이 없는 경우
        if (target_parent == nullptr) {
            root = nullptr; // 루트를 nullptr로 설정한다.
        } else if (target_parent->left == target) {
            target_parent->left = nullptr; // 부모 노드의 왼쪽 자식 포인터를 nullptr로 설정한다.
        } else {
            target_parent->right = nullptr; // 부모 노드의 오른쪽 자식 포인터를 nullptr로 설정한다.
        }
        pthread_mutex_unlock(&target->node_lock); // 락 해제
        delete target; // 삭제 대상 노드를 삭제한다.
        
    } else if (target->left == nullptr || target->right == nullptr) {
        // Case 2: 삭제 대상 노드가 하나의 자식만 가지고 있는 경우
        FineNode* child = (target->left != nullptr) ? static_cast<FineNode*>(target->left) 
        : static_cast<FineNode*>(target->right);
        if (target_parent == nullptr) {
            root = child; // 삭제 대상 노드의 부모가 없는 경우 루트를 자식으로 설정한다.
        } else if (target_parent->left == target) { 
            target_parent->left = child; // 부모 노드의 왼쪽 자식 포인터를 자식으로 설정한다.
        } else { 
            target_parent->right = child; // 부모 노드의 오른쪽 자식 포인터를 자식으로 설정한다.
        }
        pthread_mutex_unlock(&target->node_lock); // 락 해제
        delete target; // 삭제 대상 노드를 삭제한다.
    } else {
        // Case 3: 삭제 대상 노드가 두 개의 자식을 모두 가지고 있는 경우
        FineNode* min = static_cast<FineNode*>(target->right);
        FineNode* min_parent = target;

        while (min->left != nullptr) {
            pthread_mutex_lock(&min->node_lock); // 가장 작은 노드에 락을 건다.
            pthread_mutex_unlock(&min_parent->node_lock); // 이전 부모 노드의 락을 해제한다.
            min_parent = min; // 새로운 부모 노드 포인터를 설정한다.
            min = static_cast<FineNode*>(min->left); // 왼쪽 자식 노드로 이동한다.
        }

        // 가장 작은 노드의 데이터를 삭제 대상 노드로 복사한다.
        target->key = min->key;
        target->value = min->value;
        target->upd_cnt = min->upd_cnt;

        if (min_parent->left == min) {
            min_parent->left = min->right; // 가장 작은 노드의 오른쪽 자식을 부모의 왼쪽 자식으로 설정한다.
        } else {
            min_parent->right = min->right; // 가장 작은 노드의 오른쪽 자식을 부모의 오른쪽 자식으로 설정한다.
        }

        pthread_mutex_unlock(&min_parent->node_lock); // 가장 작은 노드의 부모 노드의 락을 해제한다.
        delete min; // 가장 작은 노드를 삭제한다.
        pthread_mutex_unlock(&target->node_lock); // 삭제 대상 노드의 락을 해제한다.
        
    }
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

