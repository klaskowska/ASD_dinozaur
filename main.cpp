#include <iostream>

using namespace std;

// reprezentuje kod jako drzewo
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

// *** FUNKCJE POMOCNICZE ***

// gettery:

int get_count(Node *v) {
    return v == NULL ? 0 : v->count;
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


// FUNKCJE AKTUALIZUJACE STAN WEZLOW

// zmienia stan wierzcholka na odwrocony, przy czym zmienia jego atrybuty na wlasciwe,
// ale nie odwraca jego poddrzewa
void make_reversed(Node *v) {
    if (v == NULL)
        return;

    v->is_reverse = !v->is_reverse;
    char first = v->first_letter;
    int first_length = v->first_segment_length;
    v->first_letter = v->last_letter;
    v->first_segment_length = v->last_segment_length;
    v->last_letter = first;
    v->last_segment_length = first_length;
}

// jezeli wierzcholek ma stan odwrocony, to go odpakowuje,
// czyli odwraca jego dzieci i zmienia ich stan na odwrocony
void unpack(Node *v) {
    if (v != NULL && v->is_reverse) {
        v->is_reverse = false;

        Node *tmp = v->left;
        v->left = v->right;
        v->right = tmp;

        make_reversed(v->left);
        make_reversed(v->right);
    }
}

// aktualizuje atrybuty wierzcholka;
// zakladam, ze v jest odpakowany
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



// dostaje odpakowane v
Node *kth_node(Node *v, int k) {
    if (k > v->count)
        return kth_node(v, v->count);

    int count_left = (v->left != NULL ? v->left->count : 0);
    if (k <= count_left) {
        unpack(v->left);
        return kth_node(v->left, k);
    }
    else if (k == count_left + 1)
        return v;
    else {
        unpack(v->right);
        return kth_node(v->right, k - count_left - 1);
    }
}

// FUNKCJE WYKONUJACE ROTACJE

// dostaje odpakowany root i nieodpakowane v;
// wychodzi z odpakowanym root
void left_rotation(Node **root, Node *v) {
    unpack(v);

    Node *w = v->right;
    Node *p = v->up;

    if (w != NULL) {
        unpack(w);

        v->right = w->left;
        if (v->right != NULL )
            v->right->up = v;

        w->left = v;
        w->up = p;
        v->up = w;

        unpack(p);
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

// dostaje odpakowany root i nieodpakowane v;
// wychodzi z odpakowanym root
void right_rotation(Node **root, Node *v) {
    unpack(v);

    Node *w = v->left;
    Node *p = v->up;

    if (w != NULL) {
        unpack(w);
        v->left = w->right;
        if (v->left != NULL) v->left->up = v;

        w->right = v;
        w->up = p;
        v->up = w;

        unpack(p);
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

// dokonuje rotacji tak, aby element k-ty z kolei znalazł się w korzeniu
// (jeśli k > liczba_wierzcholkow, to w korzeniu znajdzie sie ostatni wierzcholek)
// dostaje nieodpakowany root, wychodzi z odpakowanym root
void splay (Node **root, int k) {
    if (*root != NULL) {
        unpack(*root);

        Node *x = kth_node(*root, k);

        bool ready = false;

        while (!ready) {
            if (x->up == NULL) {
                ready = true;
            }

            else if (x->up->up == NULL) {
                if (x->up->left == x)
                    right_rotation(root, x->up);
                else
                    left_rotation(root, x->up);
                ready = true;
            }

            else if (x->up->up->left == x->up && x->up->left == x) {
                right_rotation( root, x->up->up );
                right_rotation( root, x->up );
            }

            else if (x->up->up->right == x->up && x->up->right == x) {
                left_rotation( root, x->up->up );
                left_rotation( root, x->up );
            }

            else if (x->up->right == x) {
                left_rotation( root, x->up );
                right_rotation( root, x->up );
            }

            else {
                right_rotation( root, x->up );
                left_rotation( root, x->up );
            }
        }
    }
}


// zwraca nieodpakowane poddrzewo zawierajace wierzholki z przedzialu [j,k]
Node *find_subtree(Node **root, int j, int k) {
    if (k - j + 1 == (*root)->count) {
        return *root;
    }

    if (k == (*root)->count) {
        splay(root, j - 1);
        return (*root)->right;
    }

    splay(root, k + 1);

    if (j == 1) {
        return (*root)->left;
    }
    else {
        (*root)->left->up = NULL;
        splay(&(*root)->left, j - 1);
        (*root)->left->up = *root;
        return (*root)->left->right;
    }
}

// zwraca dlugosc najdluzszego fragmentu tych samych liter na przedziale [j,k]
int max_segment(Node **root, int j, int k) {
    Node *sub = find_subtree(root, j, k);
    return sub->max_segment_length;
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


// WLASCIWE FUNKCJE

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

Node *operation_N(Node *root, int j, int k) {
    if (j == k) {
        cout << "1\n";
    }
    else {
        cout << max_segment(&root, j, k) << '\n';
    }
    return root;
}


Node *operation_P(Node *root, int j, int k, int l) {
    if (k - j + 1 == root->count) {
        return root;
    }

    Node *subtree = find_subtree(&root, j, k);
    subtree->up = NULL;


    if (k == root->count) {
        root->right = NULL;
        update(root);
    }
    else {
        if (j == 1) {
            root->left = NULL;
            update(root);
        } else {
            root->left->right = root;
            root = root->left;
            root->right->left = NULL;
            root->right->up = root;
            update(root->right);
            root->up = NULL;
            update(root);
        }
    }
    root = insert_subtree(root, subtree, l);
    return root;
}

Node *operation_O(Node *root, int j, int k) {
    if (j == k)
        return root;

    Node *subtree = find_subtree(&root, j, k);

    make_reversed(subtree);

    Node *father = subtree->up;

    while (father != NULL) {
        update(father);
        father = father->up;
    }

    return root;
}

void delete_tree(Node *root) {
    if (root != NULL) {
        delete_tree(root->left);
        delete_tree(root->right);
    }
    delete root;
}


int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    // wczytywanie
    int n, m, j, k, l;
    char type;
    std::cin >> n;
    std::cin >> m;

    char code[n];
    std::cin >> code;
    Node *root = create_tree(n, code);

    // wykonywanie polecen
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
                root = operation_N(root, j, k);
        }
    }

    delete_tree(root);
    return 0;
}
