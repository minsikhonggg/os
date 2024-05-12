#include "bst.h"

#ifndef BST_IMPL_H
#define BST_IMPL_H

/**
 * @brief BST without lock
 * DefaultBST의 함수를 오버라이드하여, 클래스를 완성한다.
 * 구현에 필요한 멤버 변수/함수를 추가하여 구현한다.
 */
class BST : public DefaultBST {
private:
    // 멤버 변수 추가 선언 가능
    Node* root; // 루트 노드 포인터

public:
    // 멤버 함수 추가 선언 가능
    BST(); 
    ~BST(); 
    void insert(int key, int value) override;
    int lookup(int key) override;
    void remove(int key) override;
    void traversal(KVC* arr) override;
    void traversalRecursive(Node* node, KVC* arr, int& index);
};

/**
 * @brief BST with coarse-grained lock
 * BST 전체를 critical section으로 가정하여, 하나의 lock으로 이를 관리한다.
 * DefaultBST의 함수를 오버라이드하여, 클래스를 완성한다.
 * 구현에 필요한 멤버 변수/함수를 추가하여 구현한다.
 */
class CoarseBST : public DefaultBST {
private:
    // 멤버 변수 추가 선언 가능
    Node* root; // 루트 노드 포인터
    pthread_mutex_t mutex_lock; // 뮤텍스 락

public:
    // 멤버 함수 추가 선언 가능
    CoarseBST();
    ~CoarseBST();
    void insert(int key, int value) override;
    int lookup(int key) override;
    void remove(int key) override;
    void traversal(KVC* arr) override;
    void traversalRecursive(Node* node, KVC* arr, int& index);
};

/// @brief FineBST는 FineNode를 정의하여 사용하길 권장한다.
struct FineNode : public Node {
    // 멤버 변수 추가 가능
    pthread_mutex_t node_lock; // FineNode에 대한 fine-grained lock

    pthread_cond_t insert_cond;  // insert 작업이 대기할 조건 변수
    pthread_cond_t remove_cond;  // remove 작업이 대기할 조건 변수
    bool is_being_modified;   // 현재 수정 중인지 표시

    FineNode(int key, int value, int upd_cnt, Node* left, Node* right) 
    : Node{key, value, upd_cnt, left, right}, is_being_modified(false) {
        pthread_mutex_init(&node_lock, nullptr);
        pthread_cond_init(&insert_cond, nullptr);
        pthread_cond_init(&remove_cond, nullptr);
    }

    ~FineNode() {
        pthread_mutex_destroy(&node_lock);
        pthread_cond_destroy(&insert_cond);
        pthread_cond_destroy(&remove_cond);
    }
};

/**
 * @brief BST with fine-grained lock
 * BST 내부의 critical section을 Node 단위로 lock으로 관리한다.
 * Node를 상속받은 FineNode를 정의하여 사용하길 권장한다.
 * DefaultBST의 함수를 오버라이드하여, 클래스를 완성한다.
 * 구현에 필요한 멤버 변수/함수를 추가하여 구현한다.
 */
class FineBST : public DefaultBST {
	private:
		// 멤버 변수 추가 선언 가능
        FineNode* root; // 루트 노드 포인터
        
	public:
        FineBST();
        ~FineBST();
		void insert(int key, int value) override;
		int lookup(int key) override;
		void remove(int key) override;
		void traversal(KVC* arr) override;
        void traversalRecursive(FineNode* node, KVC* arr, int& index);
};
#endif