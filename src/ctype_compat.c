/*
 * ctype_compat.c -- C99/POSIX ctype functions missing from Solaris 7
 *
 * Part of libsolcompat
 */

/* isblank() -- C99 character classification function
 * Returns non-zero if c is a blank character (space or horizontal tab).
 */
int isblank(int c)
{
    return (c == ' ' || c == '\t');
}
