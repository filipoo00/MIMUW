#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "seq.h"
#include <stdio.h>

struct abstract_class {
	int counter;
	char* s;
};
typedef struct abstract_class a_class;

struct seq {
	int w;
	struct seq* lchild;
	struct seq* mchild;
	struct seq* rchild;
	a_class* class;
};
typedef struct seq seq_t;

a_class* class_new(char* s) {
	a_class* new = (a_class*)malloc(sizeof(a_class));
	if (new == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	new->counter = 1;
	new->s = s;

	return new;
}

seq_t* seq_new(void) {
	seq_t* new = (seq_t*)malloc(sizeof(seq_t));
	if (new == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	new->w = -1;
	new->lchild = NULL;
	new->mchild = NULL;
	new->rchild = NULL;
	new->class = NULL;

	return new;
}

void seq_delete_all(seq_t* p) {
    if (p != NULL) {
        seq_delete_all(p->lchild);
        seq_delete_all(p->mchild);
        seq_delete_all(p->rchild);
        if (p->class != NULL) {
            if (p->class->counter == 1) {
                free(p->class->s);
                free(p->class);
                p->class = NULL;
            }
            else if (p->class->counter > 1) {
                (p->class->counter)--;
                p->class = NULL;
            }
        }
        free(p);
    }
}

void seq_delete(seq_t* p) {
	if (p != NULL)
		seq_delete_all(p);
}

int seq_bad(char const* s) {
	int i = 0;

	if (s == NULL || *s == '\0') {
		return -1;
	}

	while (*(s + i) - 48 == 0 || *(s + i) - 48 == 1 || *(s + i) - 48 == 2) {
		i++;
    }
	if (*(s + i) != '\0') {
		return -1;
    }

	return 1;
}

seq_t* seq_walk(seq_t* p, char const* s, int step, int* l) {
	int i = 0;
	int stop = 0;

	while (*(s + i + step) != '\0' && stop == 0) {
		if (*(s + i) - 48 == 0) {
			if (p->lchild != NULL) {
				p = p->lchild;
		    }
			else {
				stop = 1;
			}
		}
		else if (*(s + i) - 48 == 1) {
			if (p->mchild != NULL) {
				p = p->mchild;
			}
			else {
				stop = 1;
			}
		}
		else if (*(s + i) - 48 == 2) {
			if (p->rchild != NULL) {
				p = p->rchild;
			}
			else {
				stop = 1;
			}
		}
		i++;
		if (l != NULL) {
			(*l)++;
		}
	}

	if (stop == 0 && step == 0 && l != NULL) {
		return NULL;
	}
	else if (step == 1 && stop == 1) {
		return NULL;
	}
	else if (stop == 1 && l == NULL) {
		return NULL;
	}

	return p;
}

int seq_add_new(seq_t* p, char const* s) {
	seq_t* new = (seq_t*)malloc(sizeof(seq_t));
	if (new == NULL) {
		return -1;
	}
	new->lchild = NULL;
	new->mchild = NULL;
	new->rchild = NULL;
	new->class = NULL;
	if (*s - 48 == 0) {
		new->w = 0;
		p->lchild = new;
	}
	else if (*s - 48 == 1) {
		new->w = 1;
		p->mchild = new;
	}
	else if (*s - 48 == 2) {
		new->w = 2;
		p->rchild = new;
	}

	return 1;
}

void seq_add_delete(seq_t* p) {
    if (p != NULL) {
        seq_add_delete(p->lchild);
        seq_add_delete(p->mchild);
        seq_add_delete(p->rchild);
        free(p);
    }
}

int seq_add(seq_t* p, char const* s) {
	if (p == NULL || seq_bad(s) == -1) {
		errno = EINVAL;
		return -1;
	}

	int l[1] = { 0 };
	seq_t* h = seq_walk(p, s, 0, l);
	seq_t* h2 = h;
	l[0]--;

	if (h == NULL) {
		return 0;
    }

    int j = l[0];

	while (*(s + l[0]) != '\0') {
		int check = seq_add_new(h, s + l[0]);
		if (check == -1) {
			errno = ENOMEM;
			if (*(s + j) - 48 == 0) {
			    seq_add_delete(h2->lchild);
			    h2->lchild = NULL;
		    }
		    else if (*(s + j) - 48 == 1) {
		        seq_add_delete(h2->mchild);
		        h2->mchild = NULL;
		    }
		    else if (*(s + j) - 48 == 2) {
		        seq_add_delete(h2->rchild);
		        h2->rchild = NULL;
		    }

			return -1;
		}

		if (*(s + l[0]) - 48 == 0) {
			h = h->lchild;
		}
		else if (*(s + l[0]) - 48 == 1) {
			h = h->mchild;
		}
		else if (*(s + l[0]) - 48 == 2) {
			h = h->rchild;
		}

		l[0]++;
	}

	return 1;
}

void seq_class_remove(seq_t* p) {
	if (p != NULL) {
		if (p -> class != NULL) {
			if (p->class->counter == 1) {
				a_class* c = p->class;
				free(p->class->s);
				p->class = NULL;
				free(c);
			}
			else if (p->class->counter > 1) {
			    (p->class->counter)--;
				p->class = NULL;
			}
		}
		seq_class_remove(p->lchild);
		seq_class_remove(p->mchild);
		seq_class_remove(p->rchild);
		free(p);
	}
}

int seq_remove(seq_t* p, char const* s) {
	if (p == NULL || seq_bad(s) == -1) {
		errno = EINVAL;
		return -1;
	}

	int l[1] = { 0 };
	seq_t* h1 = seq_walk(p, s, 1, l);

	if (h1 == NULL) {
		return 0;
    }

	if (*(s + l[0]) - 48 == 0) {
		if (h1->lchild == NULL) {
			return 0;
		}

		seq_t* h2 = h1->lchild;
		h1->lchild = NULL;
		seq_class_remove(h2);
	}
	else if (*(s + l[0]) - 48 == 1) {
		if (h1->mchild == NULL) {
			return 0;
		}

		seq_t* h2 = h1->mchild;
		h1->mchild = NULL;
		seq_class_remove(h2);
	}
	else if (*(s + l[0]) - 48 == 2) {
		if (h1->rchild == NULL) {
			return 0;
		}

		seq_t* h2 = h1->rchild;
		h1->rchild = NULL;
		seq_class_remove(h2);
	}

	return 1;
}

int seq_valid(seq_t* p, char const* s) {
	if (p == NULL || seq_bad(s) == -1) {
		errno = EINVAL;
		return -1;
	}
	seq_t* h = seq_walk(p, s, 0, NULL);
	if (h == NULL) {
		return 0;
	}

	return 1;
}

int seq_set_name(seq_t* p, char const* s, char const* n) {
	if (p == NULL || seq_bad(s) == -1 || n == NULL || *n == '\0') {
		errno = EINVAL;
		return -1;
	}

	char* n_copy =(char*)malloc(strlen(n) + 1);
	if (n_copy == NULL) {
	    errno = ENOMEM;
	    free(n_copy);
	    return -1;
    }
	strcpy(n_copy, n);

	seq_t* h = seq_walk(p, s, 0, NULL);

	if (h == NULL) {
	    free(n_copy);
		return 0;
	}

	if (h->class == NULL) {
		h->class = class_new(n_copy);
		if (h->class == NULL) {
		    errno = ENOMEM;
		    free(n_copy);
		    return -1;
        }

		return 1;
	}

	if (strcmp(n_copy, h->class->s) == 0) {
	    free(n_copy);
		return 0;
	}
	else {
	    char* b = h->class->s;
	    h->class->s = n_copy;
	    free(b);
    }

	return 1;
}

char const* seq_get_name(seq_t* p, char const* s) {
	if (p == NULL || seq_bad(s) == -1) {
		errno = EINVAL;
		return NULL;
	}

	seq_t* h = seq_walk(p, s, 0, NULL);

	if (h == NULL) {
		errno = 0;
		return NULL;
	}

	if (h->class != NULL && *(h->class->s) != '\0')
		return h->class->s;

	errno = 0;
	return NULL;
}

void class_change(seq_t* p, a_class* old, a_class* new) {
	if (p != NULL) {
		if (p->class == old) {
			p->class->s = NULL;
			p->class = new;
			(p->class->counter)++;
		}

		class_change(p->lchild, old, new);
		class_change(p->mchild, old, new);
		class_change(p->rchild, old, new);
	}
}

int seq_equiv(seq_t* p, char const* s1, char const* s2) {
	seq_t* t = p;
	if (p == NULL || seq_bad(s1) == -1 || seq_bad(s2) == -1) {
		errno = EINVAL;
		return -1;
	}

	if (strcmp(s1, s2) == 0) {
		return 0;
	}

	seq_t* h1 = seq_walk(p, s1, 0, NULL);
	seq_t* h2 = seq_walk(p, s2, 0, NULL);

	if (h1 == NULL || h2 == NULL) {
		return 0;
    }

	if (h1->class == NULL && h2->class != NULL) {
		h1->class = h2->class;
		(h1->class->counter)++;
		return 1;
	}

	if (h1->class != NULL && h2->class == NULL) {
		h2->class = h1->class;
		(h1->class->counter)++;
		return 1;
	}

	if (h1->class == NULL && h2->class == NULL) {
		char* s = (char*)malloc(strlen("") + 1);
		if (s == NULL) {
			errno = ENOMEM;
			return -1;
		}
		strcpy(s, "");
		a_class* c = class_new(s);
		if (c == NULL) {
		    errno = ENOMEM;
		    free(s);
		    return -1;
	    }
		c->counter = 2;
		h1->class = c;
		h2->class = c;
		return 1;
	}

	if (h1->class != NULL && h2->class != NULL) {
		if (h1->class == h2->class) {
			return 0;
		}
		else if (h1->class != h2->class) {
			if (strcmp(h1->class->s, h2->class->s) == 0) {
				a_class* c1 = h1->class;
				a_class* c2 = h2->class;
				a_class* c3 = class_new(h1->class->s);
				if (c3 == NULL) {
					errno = ENOMEM;
					return -1;
				}
				c3->counter = 0;
				class_change(t, c1, c3);
				class_change(t, c2, c3);
			    free(c1);
				free(c2);
			}
			else {
				char* s = malloc(strlen(h1->class->s) + strlen(h2->class->s) + 1);
				if (s == NULL) {
					errno = ENOMEM;
					return -1;
				}
				strcpy(s, h1->class->s);
				strcat(s, h2->class->s);
				a_class* c = class_new(s);
				if (c == NULL) {
					errno = ENOMEM;
					free(s);
					return -1;
				}
				c->counter = 0;
				a_class* c1 = h1->class;
				a_class* c2 = h2->class;
				char* s1 = c1->s;
				char* s2 = c2->s;
				class_change(t, c1, c);
				class_change(t, c2, c);
				free(s1);
				free(s2);
				free(c1);
				free(c2);
			}
		}
	}

	return 1;
}
