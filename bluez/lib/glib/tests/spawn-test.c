/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#include <glib.h>
#include <glib/gstdio.h>

#ifdef G_OS_UNIX
#include <unistd.h>
#endif

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#define pipe(fds) _pipe(fds, 4096, _O_BINARY)
#endif

#ifdef G_OS_WIN32
static gchar *dirname = NULL;
#endif

#ifdef G_OS_WIN32
static char *
get_system_directory (void)
{
  wchar_t path_utf16[MAX_PATH] = {0};
  char *path = NULL;

  if (!GetSystemDirectoryW (path_utf16, G_N_ELEMENTS (path_utf16)))
    g_error ("%s failed with error code %u", "GetSystemWindowsDirectory",
             (unsigned int) GetLastError ());

  path = g_utf16_to_utf8 (path_utf16, -1, NULL, NULL, NULL);
  g_assert_nonnull (path);

  return path;
}
#endif

static void
test_spawn_basics (void)
{
  gboolean result;
  GError *err = NULL;
  gchar *output = NULL;
  gchar *erroutput = NULL;
#ifdef G_OS_WIN32
  int n;
  char buf[100];
  int pipedown[2], pipeup[2];
  gchar **argv = NULL;
  gchar *system_directory;
  gchar spawn_binary[1000] = {0};
  gchar full_cmdline[1000] = {0};
  const LCID old_lcid = GetThreadUILanguage ();
  const unsigned int initial_cp = GetConsoleOutputCP ();

  SetConsoleOutputCP (437); /* 437 means en-US codepage */
  SetThreadUILanguage (MAKELCID (MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
  system_directory = get_system_directory ();

  g_snprintf (spawn_binary, sizeof (spawn_binary),
              "%s\\spawn-test-win32-gui.exe", dirname);
#endif

  err = NULL;
  result =
    g_spawn_command_line_sync ("nonexistent_application foo 'bar baz' blah blah",
                               NULL, NULL, NULL, &err);
  g_assert_false (result);
  g_assert_error (err, G_SPAWN_ERROR, G_SPAWN_ERROR_NOENT);
  g_clear_error (&err);

  err = NULL;
  result =
    g_spawn_command_line_async ("nonexistent_application foo bar baz \"blah blah\"",
                                &err);
  g_assert_false (result);
  g_assert_error (err, G_SPAWN_ERROR, G_SPAWN_ERROR_NOENT);
  g_clear_error (&err);

  err = NULL;
#ifdef G_OS_UNIX
  result = g_spawn_command_line_sync ("/bin/sh -c 'echo hello'",
                                      &output, NULL, NULL, &err);
  g_assert_no_error (err);
  g_assert_true (result);
  g_assert_cmpstr (output, ==, "hello\n");

  g_free (output);
  output = NULL;
#endif

  /* Running sort synchronously, collecting its output. 'sort' command
   * is selected because it is non-builtin command on both unix and
   * win32 with well-defined stdout behaviour.
   * On win32 we use an absolute path to the system-provided sort.exe
   * because a different sort.exe may be available in PATH. This is
   * important e.g for the MSYS2 environment, which provides coreutils
   * sort.exe
   */
  g_file_set_contents ("spawn-test-created-file.txt",
                       "line first\nline 2\nline last\n", -1, &err);
  g_assert_no_error(err);

#ifndef G_OS_WIN32
  result = g_spawn_command_line_sync ("sort spawn-test-created-file.txt",
                                      &output, &erroutput, NULL, &err);
#else
  g_snprintf (full_cmdline, sizeof (full_cmdline),
              "'%s\\sort.exe' spawn-test-created-file.txt", system_directory);
  result = g_spawn_command_line_sync (full_cmdline, &output, &erroutput, NULL, &err);
#endif
  g_assert_no_error (err);
  g_assert_true (result);
  g_assert_nonnull (output);
  if (strchr (output, '\r') != NULL)
    g_assert_cmpstr (output, ==, "line 2\r\nline first\r\nline last\r\n");
  else
    g_assert_cmpstr (output, ==, "line 2\nline first\nline last\n");
  g_assert_cmpstr (erroutput, ==, "");

  g_free (output);
  output = NULL;
  g_free (erroutput);
  erroutput = NULL;

#ifndef G_OS_WIN32
  result = g_spawn_command_line_sync ("sort non-existing-file.txt",
                                      NULL, &erroutput, NULL, &err);
#else
  g_snprintf (full_cmdline, sizeof (full_cmdline),
              "'%s\\sort.exe' non-existing-file.txt", system_directory);
  result = g_spawn_command_line_sync (full_cmdline, NULL, &erroutput, NULL, &err);
#endif
  g_assert_no_error (err);
  g_assert_true (result);
#ifndef G_OS_WIN32
  /* Test against output of coreutils sort */
  g_assert_true (g_str_has_prefix (erroutput, "sort: "));
  g_assert_nonnull (strstr (erroutput, g_strerror (ENOENT)));
#else
  /* Test against output of windows sort */
  {
    gchar *file_not_found_message = g_win32_error_message (ERROR_FILE_NOT_FOUND);
    g_test_message ("sort output: %s\nExpected message: %s", erroutput, file_not_found_message);
    g_assert_nonnull (strstr (erroutput, file_not_found_message));
    g_free (file_not_found_message);
  }
#endif
  g_free (erroutput);
  erroutput = NULL;
  g_unlink ("spawn-test-created-file.txt");

#ifdef G_OS_WIN32
  g_test_message ("Running spawn-test-win32-gui in various ways.");

  g_test_message ("First asynchronously (without wait).");
  g_snprintf (full_cmdline, sizeof (full_cmdline), "'%s' 1", spawn_binary);
  result = g_spawn_command_line_async (full_cmdline, &err);
  g_assert_no_error (err);
  g_assert_true (result);

  g_test_message ("Now synchronously, collecting its output.");
  g_snprintf (full_cmdline, sizeof (full_cmdline), "'%s' 2", spawn_binary);
  result =
    g_spawn_command_line_sync (full_cmdline, &output, &erroutput, NULL, &err);

  g_assert_no_error (err);
  g_assert_true (result);
  g_assert_cmpstr (output, ==, "# This is stdout\r\n");
  g_assert_cmpstr (erroutput, ==, "This is stderr\r\n");

  g_free (output);
  output = NULL;
  g_free (erroutput);
  erroutput = NULL;

  g_test_message ("Now with G_SPAWN_FILE_AND_ARGV_ZERO.");
  g_snprintf (full_cmdline, sizeof (full_cmdline),
              "'%s' this-should-be-argv-zero print_argv0", spawn_binary);
  result = g_shell_parse_argv (full_cmdline, NULL, &argv, &err);
  g_assert_no_error (err);
  g_assert_true (result);

  result = g_spawn_sync (NULL, argv, NULL, G_SPAWN_FILE_AND_ARGV_ZERO,
                         NULL, NULL, &output, NULL, NULL, &err);
  g_assert_no_error (err);
  g_assert_true (result);
  g_assert_cmpstr (output, ==, "this-should-be-argv-zero");

  g_free (output);
  output = NULL;
  g_free (argv);
  argv = NULL;

  g_test_message ("Now talking to it through pipes.");
  g_assert_cmpint (pipe (pipedown), >=, 0);
  g_assert_cmpint (pipe (pipeup), >=, 0);

  g_snprintf (full_cmdline, sizeof (full_cmdline), "'%s' pipes %d %d",
              spawn_binary, pipedown[0], pipeup[1]);

  result = g_shell_parse_argv (full_cmdline, NULL, &argv, &err);
  g_assert_no_error (err);
  g_assert_true (result);

  result = g_spawn_async (NULL, argv, NULL,
                          G_SPAWN_LEAVE_DESCRIPTORS_OPEN |
                          G_SPAWN_DO_NOT_REAP_CHILD,
                          NULL, NULL, NULL,
                          &err);
  g_assert_no_error (err);
  g_assert_true (result);
  g_free (argv);
  argv = NULL;

  g_assert_cmpint (read (pipeup[0], &n, sizeof (n)), ==, sizeof (n));
  g_assert_cmpint (read (pipeup[0], buf, n), ==, n);

  n = strlen ("Bye then");
  g_assert_cmpint (write (pipedown[1], &n, sizeof (n)), !=, -1);
  g_assert_cmpint (write (pipedown[1], "Bye then", n), !=, -1);

  g_assert_cmpint (read (pipeup[0], &n, sizeof (n)), ==, sizeof (n));
  g_assert_cmpint (n, ==, strlen ("See ya"));

  g_assert_cmpint (read (pipeup[0], buf, n), ==, n);

  buf[n] = '\0';
  g_assert_cmpstr (buf, ==, "See ya");
#endif

#ifdef G_OS_WIN32
  SetThreadUILanguage (old_lcid);
  SetConsoleOutputCP (initial_cp); /* 437 means en-US codepage */
  g_free (system_directory);
#endif
}

#ifdef G_OS_UNIX
static void
test_spawn_stdio_overwrite (void)
{
  gboolean result;
  int ret;
  GError *error = NULL;
  int old_stdin_fd = -1;
  int old_stdout_fd = -1;
  int old_stderr_fd = -1;
  char **envp = g_get_environ ();
  enum OpenState { OPENED = 0, CLOSED = 1, DONE = 2 } stdin_state, stdout_state, stderr_state, output_return_state, error_return_state;

  g_test_bug ("https://gitlab.gnome.org/GNOME/glib/-/issues/16");

  old_stdin_fd = dup (STDIN_FILENO);
  old_stdout_fd = dup (STDOUT_FILENO);
  old_stderr_fd = dup (STDERR_FILENO);

  for (output_return_state = OPENED; output_return_state != DONE; output_return_state++)
    for (error_return_state = OPENED; error_return_state != DONE; error_return_state++)
      for (stdin_state = OPENED; stdin_state != DONE; stdin_state++)
        for (stdout_state = OPENED; stdout_state != DONE; stdout_state++)
          for (stderr_state = OPENED; stderr_state != DONE; stderr_state++)
    {
        char *command_line = NULL;
        char **argv = NULL;
        gchar *standard_output = NULL;
        gchar *standard_error = NULL;

        g_test_message ("Fetching GSpawn result %s%s%s with stdin %s, stdout %s, stderr %s",
                        output_return_state == OPENED? "output" : "",
                        output_return_state == OPENED && error_return_state == OPENED? " and " : "",
                        error_return_state == OPENED? "error output" : "",
                        stdin_state == CLOSED? "already closed" : "open",
                        stdout_state == CLOSED? "already closed" : "open",
                        stderr_state == CLOSED? "already closed" : "open");

        if (stdin_state == CLOSED)
          {
            g_close (STDIN_FILENO, &error);
            g_assert_no_error (error);
          }

        if (stdout_state == CLOSED)
          {
            g_close (STDOUT_FILENO, &error);
            g_assert_no_error (error);
          }

        if (stderr_state == CLOSED)
          {
            g_close (STDERR_FILENO, &error);
            g_assert_no_error (error);
          }

        command_line = g_strdup_printf ("/bin/sh -c '%s%s%s'",
                                        output_return_state == OPENED? "echo stdout": "",
                                        output_return_state == OPENED && error_return_state == OPENED? ";" : "",
                                        error_return_state == OPENED? "echo stderr >&2": "");
        g_shell_parse_argv (command_line, NULL, &argv, &error);
        g_assert_no_error (error);

        g_clear_pointer (&command_line, g_free);

        result = g_spawn_sync (NULL,
                               argv, envp, G_SPAWN_SEARCH_PATH_FROM_ENVP,
                               NULL, NULL,
                               output_return_state == OPENED? &standard_output : NULL,
                               error_return_state == OPENED? &standard_error: NULL,
                               NULL,
                               &error);
        g_clear_pointer (&argv, g_strfreev);

        ret = dup2 (old_stderr_fd, STDERR_FILENO);
        g_assert_cmpint (ret, ==, STDERR_FILENO);

        ret = dup2 (old_stdout_fd, STDOUT_FILENO);
        g_assert_cmpint (ret, ==, STDOUT_FILENO);

        ret = dup2 (old_stdin_fd, STDIN_FILENO);
        g_assert_cmpint (ret, ==, STDIN_FILENO);

        g_assert_no_error (error);
        g_assert_true (result);

        if (output_return_state == OPENED)
          {
            g_assert_cmpstr (standard_output, ==, "stdout\n");
            g_clear_pointer (&standard_output, g_free);
          }

        if (error_return_state == OPENED)
          {
            g_assert_cmpstr (standard_error, ==, "stderr\n");
            g_clear_pointer (&standard_error, g_free);
          }
    }

  g_clear_fd (&old_stdin_fd, &error);
  g_assert_no_error (error);

  g_clear_fd (&old_stdout_fd, &error);
  g_assert_no_error (error);

  g_clear_fd (&old_stderr_fd, &error);
  g_assert_no_error (error);

  g_clear_pointer (&envp, g_strfreev);
}
#endif

int
main (int   argc,
      char *argv[])
{
  int ret_val;

#ifdef G_OS_WIN32
  dirname = g_path_get_dirname (argv[0]);
#endif

  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/spawn/basics", test_spawn_basics);
#ifdef G_OS_UNIX
  g_test_add_func ("/spawn/stdio-overwrite", test_spawn_stdio_overwrite);
#endif

  ret_val = g_test_run ();

#ifdef G_OS_WIN32
  g_free (dirname);
#endif
  return ret_val;
}
