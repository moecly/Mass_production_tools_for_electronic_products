#ifndef _PAGE_MANGER_H
#define _PAGE_MANGER_H

#ifndef NULL
#define NULL (void *)0
#endif

typedef struct page_action {
  char *name;
  int (*run)(void *params);
  struct page_action *next;
} page_action;

void register_page(page_action *page);
page_action *page(char *name);
void pages_register(void);

#endif