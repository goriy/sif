#ifndef __RECENT_H__
#define __RECENT_H__

void recent_load (void);
void recent_add (const char *text);
void recent_fill_combo (int id);
const char *recent_get_by_list (int id);

const char *recent_current_increment (void);
const char *recent_current_decrement (void);
void recent_current_reset (void);

#endif
