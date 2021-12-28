#include <iostream>

// na razie bez uwzgledniania is_reverse
// moze to po prostu zrobic jako że wyznaczamy poddrzewo i robimy od razu na nim reverse

using namespace std;

struct Node {
    char letter;
    int count;
    bool is_reverse;
    int max_segment_length;

    // pierwsza litera w podciagu reprezentowanym przez dane poddrzewo
    char first_letter;
    // dlugosc pierwszego fragmentu tych samych liter w podciagu reprezentowanym przez dane poddrzewo
    int first_segment_length;

    // ostatnia litera w podciagu reprezentowanym przez dane poddrzewo
    char last_letter;
    // dlugosc ostatniego fragmentu tych samych liter w podciagu reprezentowanym przez dane poddrzewo
    int last_segment_length;

    struct Node *up;
    struct Node *left;
    struct Node *right;

    Node(char _letter) : letter(_letter), count(1), is_reverse(false), max_segment_length(1), first_letter(_letter),
    first_segment_length(1), last_letter(_letter), last_segment_length(1), up(NULL), left(NULL), right(NULL) { }
};

void reverse(Node *v) {
    Node *tmp = v->left;
    v->left = v->right;
    v->right = tmp;
    v->is_reverse = true;
}

int get_count(Node *v) {
    return v == NULL ? 0 : v->count;
}

int max(int a, int b) {
    return a > b ? a : b;
}

// sprawdza, czy cale poddrzewo sklada sie z jednakowych liter
bool is_uniform(Node *v) {
    if (v == NULL) return true;
    return v->count == v->max_segment_length;
}

// sprawdza, czy lewe poddrzewo kończy się na literę taką jak wartosc v, badz jest puste
bool is_unified_left(Node *v) {
    if (v->left == NULL) return true;
    return v->left->last_letter == v->letter;
}

// sprawdza, czy prawe poddrzewo zaczyna się na literę taką jak wartosc v, badz jest puste
bool is_unified_right(Node *v) {
    if (v->right == NULL) return true;
    return v->right->first_letter == v->letter;
}

char get_first_letter_left(Node *v) {
    if (v->left == NULL) return v->letter;
    return v->left->first_letter;
}

char get_last_letter_right(Node *v) {
    if (v->right == NULL) return v->letter;
    return v->right->last_letter;
}

int get_max(Node *v) {
    if (v == NULL) return 1;
    return v->max_segment_length;
}

int get_first_segment_length(Node *v) {
    if (v == NULL) return 0;
    else return v->first_segment_length;
}

int get_last_segment_length(Node *v) {
    if (v == NULL) return 0;
    else return v->last_segment_length;
}

void update(Node *v) {
    v->count = 1 + get_count(v->left) + get_count(v->right);
    v->first_letter = get_first_letter_left(v);
    v->first_segment_length = get_first_segment_length(v->left);
    v->last_letter = get_last_letter_right(v);
    v->last_segment_length = get_last_segment_length(v->right);
    v->max_segment_length = max(get_max(v->left), get_max(v->right));

    int max_candidate = 1;
    if (is_unified_left(v)) {
        max_candidate += get_last_segment_length(v->left);

        if (is_uniform(v->left)) {
            v->first_segment_length++;
            if (is_unified_right(v))
                v->first_segment_length += get_first_segment_length(v->right);
        }
    }

    if (is_unified_right(v)) {
        max_candidate += get_first_segment_length(v->right);

        if (is_uniform(v->right)) {
            v->last_segment_length++;
            if (is_unified_left(v))
                v->last_segment_length += get_last_segment_length(v->left);
        }
    }

    v->max_segment_length = max(v->max_segment_length, max_candidate);
}

void left_rotation(Node *root, Node *v) {
    Node *w = v->right;
    Node *p = v->up;

    if (w != NULL) {
        v->right = w->left;
        if (v->right != NULL )
            v->right->up = v;

        w->left = v;
        w->up = p;
        v->up = w;

        if (p != NULL) {
            if (p->left == v)
                p->left = w;
            else
                p->right = w;
        }
        else root = w;

        update(v);
        update(w);
    }
}

void right_rotation(Node *root, Node *v) {
    Node *w = v->left;
    Node *v_up = v->up;

    if (w != NULL) {
        v->left = w->right;
        if (v->left != NULL) v->left->up = v;

        w->right = v;
        w->up = v_up;
        v->up = w;

        if (v_up != NULL) {
            if (v_up->left == v)
                v_up->left = v_left;
            else
                v_up->right = v_left;
        }
        else root = v_left;
    }
}

// wstawia na i-te miejsce węzeł w
Node *insert(Node *v, int i, Node *w) {
    if (v == NULL) {
        return w;
    }
    if (v->is_reverse) {
        reverse(v);
    }
    if (i <= get_count(v->left))
        v->left = insert(v->left, i, w);
    else
        v->right = insert(v->right, i - get_count(v->left) - 1, w);
    update(v);
    return v;
}

void print(Node *v) {
    if (v==NULL) {
        printf("-");
    } else {
        printf("(v=%d,c=%d,max=%d,", v->letter, v->count, v->max_segment_length);
        print(v->left);
        printf(",");
        print(v->right);
        printf(")");
    }
}

int main() {
    Node *root = NULL;
    root = insert(root, 0, new Node('A'));
    root = insert(root, 1, new Node('G'));
    root = insert(root, 2, new Node('G'));
    root = insert(root, 3, new Node('G'));
    root = insert(root, 4, new Node('A'));
    root = insert(root, 5, new Node('G'));
    print(root);
    return 0;
}
