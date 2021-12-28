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

int min(int a, int b) {
    return a < b ? a : b;
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

void left_rotation(Node **root, Node *v) {
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
        else *root = w;

        update(v);
        update(w);
    }
}

void right_rotation(Node **root, Node *v) {
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
        else *root = w;

        update(v);
        update(w);
    }
}

Node *kth_node(Node *v, int k) {
    if (k > v->count)
        return kth_node(v, v->count);

    int count_left = (v->left != NULL ? v->left->count : 0);
    if (k <= count_left)
        return kth_node(v->left, k);
    else if (k == count_left + 1)
        return v;
    else
        return kth_node(v->right, k - count_left - 1);
}

void print(Node *v) {
    if (v==NULL) {
        printf("-");
    } else {
        printf("(v=%c,c=%d,max=%d,f=%c,f_c=%d,l=%c,l_c=%d, ", v->letter, v->count, v->max_segment_length
                , v->first_letter, v->first_segment_length, v->last_letter, v->last_segment_length);
        print(v->left);
        printf(",");
        print(v->right);
        printf(")");
    }
}

void splay (Node **root, int index) {


    // Poszukujemy węzła o kluczu k, poczynając od korzenia
    if (*root != NULL) {
        Node *x = kth_node(*root, index);

        while (true)           // W pętli węzeł x przesuwamy do korzenia
        {
            if (x->up == NULL) break;   // x jest korzeniem, kończymy

            if (x->up->up == NULL)
            {                     // Ojcem x jest korzeń. Wykonujemy ZIG
                if (x->up->left == x) right_rotation(root, x->up);
                else left_rotation(root, x->up);
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

    splay(&v, i);
    w->up = v;
    w->left = v->left;
    v->left = w;
    if (w->left != NULL) {
        w->left->up = w;
    }

    update(w);
    update(v);
    return v;
}

Node *remove(Node *root, int index, Node **deleted) {
    splay(&root, index);
    Node *l = root->left;
    Node *r = root->right;
    root->up = NULL;
    root->left = NULL;
    root->right = NULL;
    *deleted = root;
    root = NULL;

    if (l == NULL) {
        return r;
    }
    if (r == NULL) {
        return l;
    }

    l->up = NULL;
    splay(&l, index);
    l->right = r;
    r->up = l;
    root = l;
    update(l);

    return root;
}



Node *create_tree(size_t code_length, char *code) {
    if (code_length <= 0)
        return NULL;
    Node *root;
    size_t i = code_length / 2;
    root = new Node(code[i]);
    root->left = create_tree(i, code);
    if (root->left != NULL)
        root->left->up = root;
    root->right = create_tree(code_length - i - 1, code + i + 1);
    if (root->right != NULL)
        root->right->up = root;
    update(root);
    return root;
}

void update_length(Node *root, bool *is_value, int *max_length, char *last_letter, int *last_segment_length) {
    if (*is_value && *last_letter == root->first_letter) {
        *max_length = max(*max_length, *last_segment_length + root->first_segment_length);
        if (is_uniform(root)) {
            *last_segment_length += root->count;
        }
        else {
            *last_letter = root->last_letter;
            *last_segment_length = root->last_segment_length;
        }
    }
    else {
        *last_letter = root->last_letter;
        *last_segment_length = root->last_segment_length;
    }
    *max_length = max(*max_length, root->max_segment_length);
    *is_value = true;
}

void max_segment_length_interval_helper(Node *root, int j, int k, bool *is_value, int *max_length, char *last_letter,
                                        int *last_segment_length) {
    if (k - j + 1 == root->count) {
        update_length(root, is_value, max_length, last_letter, last_segment_length);
    }
    else {
        if (j <= root->left->count) {
            max_segment_length_interval_helper(root->left, j, min(k, root->left->count), is_value, max_length, last_letter, last_segment_length);
        }
        if (j <= root->left->count + 1 && k >= root->left->count + 1) {
            Node *tmp = new Node(root->letter);
            update_length(tmp, is_value, max_length, last_letter, last_segment_length);
            delete tmp;
        }
        if (k > root->left->count + 1) {
            max_segment_length_interval_helper(root->right, max(1, j - root->left->count - 1), k - root->left->count - 1, is_value, max_length, last_letter, last_segment_length);
        }
    }
}

int max_segment_length_interval(Node *root, int j, int k) {
    bool is_value = false;
    int max_length = 1;
    int last_segment_length;
    char last_letter;
    max_segment_length_interval_helper(root, j, k, &is_value, &max_length, &last_letter, &last_segment_length);
    return max_length;
}

void operation_N(Node *root, int j, int k) {
    cout << max_segment_length_interval(root, j, k);
}

Node *insert_subtree(Node *root, Node *deleted, int l) {
    splay(&root, l);

    if (l == 1) {
        root->left = deleted;
        deleted->up = root;
        update(root);
    }
    else if (l == root->count + 1) {
        root->right = deleted;
        deleted->up = root;
        update(root);
    }
    else {
        splay(&deleted, 1);
        deleted->left = root->left;
        deleted->left->up = deleted;
        deleted->up = root;
        root->left = deleted;
        update(deleted);
        update(root);
    }

    return root;
}

Node *operation_P(Node *root, int j, int k, int l) {
    if (k - j + 1 == root->count) {
        return root;
    }

    splay(&root, k + 1);

    Node *deleted;
    if (j == 1) {
        deleted = root->left;
        deleted->up = NULL;
        root->left = NULL;
        update(root);
    }
    else {
        root->left->up = NULL;
        splay(&root->left, j - 1);
        root->left->up = root;

        deleted = root->left->right;
        deleted->up = NULL;
        root->left->right = root;
        root = root->left;
        root->right->left = NULL;
        root->right->up = root;
        update(root->right);
        root->up = NULL;
        update(root);
    }
    root = insert_subtree(root, deleted, l);
    return root;
}

Node *find_subtree(Node *root, int j, int k) {
    if (k - j + 1 == root->count) {
        return root;
    }

    splay(&root, k + 1);

    if (j == 1) {
        return root->left;
    }
    else {
        root->left->up = NULL;
        splay(&root->left, j - 1);
        root->left->up = root;
        return root->left->right;
    }
}

void reverse_tree(Node *root) {
    Node *tmp = root->right;
    root->right = root->left;
    root->left = tmp;

    if (root->left != NULL) {
        reverse_tree(root->left);
    }
    if (root->right != NULL) {
        reverse_tree(root->right);
    }
}

Node *operation_O(Node *root, int j, int k) {
    Node *subtree = find_subtree(root, j, k);
    Node *father = subtree->up;

    update(father);
}

//TODO delete drzewo
int main() {
    int n, m, j, k, l;
    char type;
    std::cin >> n;
    std::cin >> m;

    char code[n];
    std::cin >> code;
    Node *root = create_tree(n, code);

//    print(root);

    for (int i = 0; i < m; i++) {
        std::cin >> type;
        std::cin >> j;
        std::cin >> k;
        switch (type) {
            case 'O':
                root = operation_O(root, j, k);
                break;
            case 'P':
                std::cin >> l;
                root = operation_P(root, j, k, l);
                break;
            case 'N':
                operation_N(root, j, k);
        }
    }

    print(root);
    return 0;
}
