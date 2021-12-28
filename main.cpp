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
    Node *p = v->up;

    if (w != NULL) {
        v->left = w->right;
        if (v->left != NULL) v->left->up = v;

        w->right = v;
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

Node *kth_node(Node *v, int k) {
    if (k > v->count)
        return kth_node(v, v->count);

    int count_left = (v->left != NULL ? v->left->count : 0);
    if (k < count_left)
        return kth_node(v->left, k);
    else if (k == count_left + 1)
        return v;
    else
        return kth_node(v->right, k - count_left - 1);
}

void splay (Node *root, int index) {
    Node *x;

    // Poszukujemy węzła o kluczu k, poczynając od korzenia
    if (root != NULL) {
        x = kth_node(root, index);

        while (true)           // W pętli węzeł x przesuwamy do korzenia
        {
            if (x->up == NULL) break;   // x jest korzeniem, kończymy

            if (x->up->up == NULL)
            {                     // Ojcem x jest korzeń. Wykonujemy ZIG
                if (x->up->left == x) right_rotation( root, x->up );
                else left_rotation( root, x->up );
                break;              // Kończymy
            }

            if( ( x->up->up->left == x->up ) && ( x->up->left == x ) )
            {                     // prawy ZIG-ZIG
                right_rotation( root, x->up->up );
                right_rotation( root, x->up );
                continue;
            }

            if( ( x->up->up->right == x->up ) && ( x->up->right == x ) )
            {                     // lewy ZIG-ZIG
                left_rotation( root, x->up->up );
                left_rotation( root, x->up );
                continue;
            }

            if( x->up->right == x )
            {                     // lewy ZIG, prawy ZAG
                left_rotation( root, x->up );
                right_rotation( root, x->up );
            }
            else
            {                     // prawy ZIG, lewy ZAG
                right_rotation( root, x->up );
                left_rotation( root, x->up );
            }
        }
    }
}

// i = 1...n
// wstawia na i-te miejsce węzeł w
Node *insert(Node *v, int i, Node *w) {
    if (v == NULL) {
        return w;
    }
    if (v->is_reverse) {
        reverse(v);
    }

    splay(v, i);
    w->up = v;
    if (get_count(v->left) == i - 1) {
        w->left = v->left;
        v->left = w;
        if (w->left != NULL) {
            w->left->up = w;
        }
    }
    else {
        w->right = v->right;
        v->right = w;
        if (w->right != NULL) {
            w->right->up = w;
        }
    }

    update(w);
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

//TODO delete drzewo
int main() {
    // problem ze splay, bo nie dodaja sie wezly
    Node *root = NULL;
    root = insert(root, 1, new Node('A'));
    root = insert(root, 2, new Node('G'));
    root = insert(root, 3, new Node('G'));
    root = insert(root, 4, new Node('G'));
    root = insert(root, 5, new Node('A'));
    root = insert(root, 6, new Node('G'));
    print(root);
    return 0;
}
