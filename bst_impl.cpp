#include "bst_impl.h"

// BST 구현
BST::BST() : root(nullptr) { 
} 

void BST::insert(int key, int value) {
    Node* newNode = new Node{key, value, 0, nullptr, nullptr}; 

    if (root == nullptr) { // 트리가 비어있을 경우
        root = newNode; // 새로운 노드를 루트로 설정
        return;
    }

    Node* current = root; 

    while (true) {
        if (key < current->key) { // 삽입할 키가 현재 노드의 키보다 작으면
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
    traversalRecursive(root, arr, index); // 루트부터 순회 시작
}

// CoarseBST 구현
// BST와 동일한 코드 + 전체 lock / 리턴 전 unlock
CoarseBST::CoarseBST() : root(nullptr) {
    pthread_mutex_init(&mutex_lock, nullptr); // 뮤텍스 초기화
}


void CoarseBST::insert(int key, int value) {

    pthread_mutex_lock(&mutex_lock); // 전체 트리 락

    Node* newNode = new Node{key, value, 0, nullptr, nullptr}; 
    if (root == nullptr) { 
        root = newNode; 
        pthread_mutex_unlock(&mutex_lock); 
        return;
    }

    Node* current = root; 

    while (true) {
        if (key < current->key) { 
            if (current->left == nullptr) { 
                current->left = newNode; 
                pthread_mutex_unlock(&mutex_lock); // 락 해제
                return;
            }
            current = current->left; 
        } else if (key > current->key) { 
            if (current->right == nullptr) { 
                current->right = newNode; 
                pthread_mutex_unlock(&mutex_lock); // 락 해제
                return;
            }
            current = current->right; 
        } else { 
            current->value += value; 
            current->upd_cnt++; 
            delete newNode; 
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

    if (root == nullptr) { 
        pthread_mutex_unlock(&mutex_lock); 
        return;
    }

    Node* parent = nullptr; 
    Node* current = root; 

    while (current != nullptr && current->key != key) {
        parent = current;
        if (key < current->key)
            current = current->left;
        else
            current = current->right;
    }

    if (current == nullptr) { 
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
    traversalRecursive(root, arr, index); // 루트부터 순회 시작
}

// FineBST 구현
FineBST::FineBST() : root(nullptr) {}


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

        while (cur->is_being_modified) {  // 수정 중인지 확인
            // remove 작업이 완료될 때까지 대기
            pthread_cond_wait(&cur->remove_cond, &cur->node_lock);
        }
        parent = cur;

        if (key < cur->key) { // 삽입할 키가 현재 키보다 작으면
            if (cur->left == nullptr) {
                cur->left = new_node;
                cur->is_being_modified = false; // 수정 상태 해제
                pthread_cond_signal(&cur->insert_cond);  // insert 완료 신호
                pthread_mutex_unlock(&cur->node_lock); // 노드 잠금 해제
                return;
            }
            cur = static_cast<FineNode*>(cur->left); // 왼쪽 자식으로 이동
        } else if (key > cur->key) { // 삽입할 키가 현재 키보다 크면
            if (cur->right == nullptr) {
                cur->right = new_node;
                cur->is_being_modified = false; // 수정 상태 해제
                pthread_cond_signal(&cur->insert_cond);  // insert 완료 신호
                pthread_mutex_unlock(&cur->node_lock); // 노드 잠금 해제
                return; 
            }
            cur = static_cast<FineNode*>(cur->right); // 오른쪽 자식으로 이동
        } else { // 이미 키가 존재하면
            cur->value += value;
            cur->upd_cnt++;
            cur->is_being_modified = false; // 수정 상태 해제
            pthread_cond_signal(&cur->insert_cond);  // insert 완료 신호           
            pthread_mutex_unlock(&cur->node_lock); // 현재 노드 잠금 해제
            return;
        }

        pthread_mutex_unlock(&parent->node_lock); // 부모 노드 잠금 해제
    }
}


int FineBST::lookup(int key) {

    FineNode* cur = root;

    if (cur == nullptr) {
        return 0;
    }

    while (true) {
        pthread_mutex_lock(&cur->node_lock); // 현재 노드 잠금

        if (key == cur->key) { // 키가 일치하면
            int value = cur->value;
            pthread_mutex_unlock(&cur->node_lock); // 잠금 해제
            return value;
        } else if (key < cur->key && cur->left != nullptr) { // 찾는 키가 작고 왼쪽 자식이 있으면
            FineNode* next = static_cast<FineNode*>(cur->left);
            pthread_mutex_unlock(&cur->node_lock); // 현재 노드 잠금 해제
            cur = next;
        } else if (key > cur->key && cur->right != nullptr) { // 찾는 키가 크고 오른쪽 자식이 있으면
            FineNode* next = static_cast<FineNode*>(cur->right);
            pthread_mutex_unlock(&cur->node_lock); // 현재 노드 잠금 해제
            cur = next;
        } else { // 키를 찾지 못하면
            pthread_mutex_unlock(&cur->node_lock); // 잠금 해제
            return 0;
        }
    }
}


void FineBST::remove(int key) {
    FineNode* cur = root;
    FineNode* parent = nullptr;
    FineNode* target = nullptr; // 제거될 목표 노드
    FineNode* target_parent = nullptr;

    if (cur == nullptr) {
        return;
    }

    // 제거할 노드 위치 찾기
    while (true) {
        pthread_mutex_lock(&cur->node_lock); 

        while (cur->is_being_modified) { // 수정 중인지 확인
            // insert 작업이 완료될 때까지 대기
            pthread_cond_wait(&cur->insert_cond, &cur->node_lock);
        }

        if (key == cur->key) { // 현재 노드가 목표 노드이면
            target = cur;
            target_parent = parent; 
            pthread_mutex_unlock(&cur->node_lock); // 잠금 해제 후 삭제 절차 진행
            break;
        } else if (key < cur->key && cur->left != nullptr) { // 키가 작으면 왼쪽으로 이동
            FineNode* next = static_cast<FineNode*>(cur->left);
            pthread_mutex_unlock(&cur->node_lock); // 현재 노드 잠금 해제
            parent = cur;
            cur = next; // 왼쪽 자식으로 이동
        } else if (key > cur->key && cur->right != nullptr) { // 키가 크면 오른쪽으로 이동
            FineNode* next = static_cast<FineNode*>(cur->right);
            pthread_mutex_unlock(&cur->node_lock); // 현재 노드 잠금 해제
            parent = cur;
            cur = next; // 오른쪽 자식으로 이동
        } else { // 키를 찾지 못하면
            cur->is_being_modified = false;
            pthread_cond_signal(&cur->remove_cond);  // remove 완료 신호
            pthread_mutex_unlock(&cur->node_lock);
            return;
        }
    }

    // 노드 제거 절차 수행
    pthread_mutex_lock(&target->node_lock); // 목표 노드 잠금

    if (target->left == nullptr && target->right == nullptr) { 
        // 자식 노드가 없는 경우
        if (target_parent == nullptr) { // 목표 노드가 루트인 경우
            root = nullptr;
        } else if (target_parent->left == target) { // 목표 노드가 왼쪽 자식인 경우
            target_parent->left = nullptr;
        } else { // 목표 노드가 오른쪽 자식인 경우
            target_parent->right = nullptr;
        }
        pthread_mutex_unlock(&target->node_lock); // 목표 노드 잠금 해제
    } else if (target->left == nullptr || target->right == nullptr) { 
        // 한 쪽 자식만 있는 경우
        FineNode* child = (target->left != nullptr) 
        ? static_cast<FineNode*>(target->left) : static_cast<FineNode*>(target->right);
        if (target_parent == nullptr) { // 목표 노드가 루트인 경우
            root = child;
        } else if (target_parent->left == target) { // 목표 노드가 왼쪽 자식인 경우
            target_parent->left = child;
        } else { // 목표 노드가 오른쪽 자식인 경우
            target_parent->right = child;
        }
        pthread_mutex_unlock(&target->node_lock); // 목표 노드 잠금 해제
        
    } else { // 두 개의 자식을 가진 경우
        
        // 오른쪽 자식에서 최소값 탐색 시작
        FineNode* min = static_cast<FineNode*>(target->right);
        FineNode* min_parent = target;

        while (min != nullptr && min->left != nullptr) { // 오른쪽 서브트리에서 최소 노드 찾기
            pthread_mutex_lock(&min->node_lock); // 최소 노드 잠금
            pthread_mutex_unlock(&min_parent->node_lock); // 부모 노드 잠금 해제
            min_parent = min; // 부모 업데이트
            if(min->left != nullptr) 
                min = static_cast<FineNode*>(min->left); // 왼쪽 자식으로 이동
        }

       if (target != min) { // check
            target->key = min->key;
            target->value = min->value;
            target->upd_cnt = min->upd_cnt;
        }

        if (min_parent->left == min) { // 최소 노드 제거
            min_parent->left = min->right;
        } else {
            min_parent->right = min->right;
        }

        pthread_mutex_unlock(&min_parent->node_lock); // 부모 노드의 잠금 해제

        min->is_being_modified = false;
        pthread_cond_signal(&min->remove_cond);  // remove 완료 신호
        
        pthread_mutex_unlock(&target->node_lock); // 목표 노드의 잠금 해제
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
    traversalRecursive(root, arr, index); // 루트부터 순회 시작
}