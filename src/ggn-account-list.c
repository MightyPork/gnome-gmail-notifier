/**
 * @file src/ggn-account-list.c The GgnAccountList object
 * @Copyright (C) 2010 Bradley A. Worley.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * 
 *     Free Software Foundation, Inc.
 *     59 Temple Place, Suite 330
 *     Boston, MA 02111-1307 USA
 **/

/**
 * SECTION:GgnAccountList
 * @short_description: The account list object
 * @include: src/ggn-account-list.h
 **/

/*
 * include our application header.
 */
#include <main.h>
#include "ggn-account-list-priv.h"
#include "ggn-account-priv.h"

/*
 * forward function definitions.
 */
static void ggn_account_list_init (GgnAccountList *self);
static void ggn_account_list_class_init (GgnAccountListClass *klass);
static void ggn_account_list_finalize (GObject *obj);

/*
 * define the gobject type and its basic functions.
 */
G_DEFINE_TYPE (GgnAccountList, ggn_account_list, G_TYPE_OBJECT);

/*
 * define the signals used.
 */
enum {
  MODIFIED,
  CHECKED,
  FAILED,
  LAST_SIGNAL
};
static guint signals[LAST_SIGNAL] = { 0 };

/**
 * ggn_account_list_default_modified_cb:
 * @alist: The account list which emitted the signal.
 *
 * This is the default "modified" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_account_list_default_modified_cb (GgnAccountList *alist) {
  /* do nothing. */
}

/**
 * ggn_account_list_default_checked_cb:
 * @alist: The account list which emitted the signal.
 *
 * This is the default "checked" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_account_list_default_checked_cb (GgnAccountList *alist) {
  /* do nothing. */
}

/**
 * ggn_account_list_default_failed_cb:
 * @alist: The account list which emitted the signal.
 *
 * This is the default "failed" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_account_list_default_failed_cb (GgnAccountList *alist) {
  /* do nothing. */
}

/**
 * ggn_account_list_account_modified:
 * @account: The account that was modified.
 * @data: The account list pointer.
 *
 * This function is called when a managed #GgnAccount emits its "modified"
 * signal.
 *
 * Returns: void.
 **/
static void ggn_account_list_account_modified (GgnAccount *account,
                                               gpointer data) {
  /* gain a reference to our account list. */
  GgnAccountList *alist = GGN_ACCOUNT_LIST (data);

  /* emit the modified signal. */
  g_signal_emit (alist, signals[MODIFIED], 0);
}

/**
 * ggn_account_list_account_updated:
 * @account: The account that was checked.
 * @data: The account list pointer.
 *
 * This function is called when a managed #GgnAccount emits its "updated"
 * signal.
 *
 * Returns: void.
 **/
static void ggn_account_list_account_updated (GgnAccount *emitter,
                                              gpointer data) {
  /* declare a few helping variables. */
  gboolean success;
  gboolean enab;
  guint state, idx, num_unread, num_new;

  /* gain a reference to our account list. */
  GgnAccountList *alist = GGN_ACCOUNT_LIST (data);

  /* let's be optimistic. */
  success = TRUE;

  /* loop through all of the accounts. */
  for (idx = 0; idx < alist->priv->accounts->len; idx++) {
    /* get the account at the given index. */
    alist->priv->account = ggn_account_list_get (alist, idx);

    /* get the account state and enabled info. */
    enab = ggn_account_get_enabled (alist->priv->account);
    state = ggn_account_get_state (alist->priv->account);

    /* get the account entry counts. */
    ggn_account_get_num_entries (alist->priv->account, &num_unread, &num_new);

    /* only look at enabled accounts. */
    if (enab == FALSE)
      continue;

    /* only emit if we are done with checking. */
    if (state == GGN_ACCOUNT_STATE_CHECKING) {
      /* reset the counters and wait for the final account to finish. */
      alist->priv->num_nonempty = 0;
      alist->priv->num_failed = 0;
      alist->priv->num_unread_entries = 0;
      alist->priv->num_new_entries = 0;
      return;
    }

    /* see if the account is outside the criteria for a successful check. */
    if (state == GGN_ACCOUNT_STATE_CHECKED) {
      /* if there are messages... */
      if (num_unread + num_new > 0) {
        /* increment the non-empty counter. */
        alist->priv->num_nonempty++;

        /* add these messages to the total entry counts. */
        alist->priv->num_unread_entries += num_unread;
        alist->priv->num_new_entries += num_new;
      }
    }
    else {
      /* we have failed to successfully check all accounts. */
      success = FALSE;

      /* increment the failure counter. */
      if (state == GGN_ACCOUNT_STATE_FAILED)
        alist->priv->num_failed++;
    }
  }

  /* emit the appropriate signal. */
  if (success == TRUE) {
    /* emit the "checked" signal. */
    g_signal_emit (alist, signals[CHECKED], 0);
  }
  else {
    /* emit the "failed" signal. */
    g_signal_emit (alist, signals[FAILED], 0);
  }
}

/**
 * ggn_account_list_accounts_clear:
 * @alist: The account list to modify.
 *
 * Clears the entries #GPtrArray of a given #GgnAccountList. private only.
 *
 * Returns: void.
 **/
void ggn_account_list_accounts_clear (GgnAccountList *alist) {
  /* define a helping variable. */
  gpointer ptr;

  /* loop until the pointer array is empty. */
  while (alist->priv->accounts->len > 0) {
    /* remove the first pointer. */
    ptr = g_ptr_array_index (alist->priv->accounts, 0);
    g_ptr_array_remove_fast (alist->priv->accounts, ptr);

    /* free the entry (pointer). */
    ggn_account_free ((GgnAccount *) ptr);
  }
}

/*
 * ggn_account_list_init:
 *
 * This function is used by the gobject library to
 * generate a new instance of our object.
 */
static void ggn_account_list_init (GgnAccountList *self) {
  /* set up the private data structure. */
  self->priv = g_new0 (GgnAccountListPrivate, 1);

  /* set the preferences object to NULL. */
  self->priv->prefs = NULL;

  /* setup the pointer array. */
  self->priv->accounts = g_ptr_array_new ();
  self->priv->account = NULL;

  /* initialize the count variables. */
  self->priv->num_nonempty = 0;
  self->priv->num_failed = 0;
  self->priv->num_unread_entries = 0;
  self->priv->num_new_entries = 0;
}

/*
 * ggn_account_list_class_init:
 *
 * This function is used by the gobject library to
 * generate a new class object of our object.
 */
static void ggn_account_list_class_init (GgnAccountListClass *klass) {
  /* setup a gobject class. */
  GObjectClass *gobj_class = G_OBJECT_CLASS (klass);

  /* set the locations of our destruction function. */
  gobj_class->finalize = ggn_account_list_finalize;

  /* setup the default signal handlers. */
  klass->modified = ggn_account_list_default_modified_cb;
  klass->checked = ggn_account_list_default_checked_cb;
  klass->failed = ggn_account_list_default_failed_cb;

  /**
   * GgnAccountList::modified:
   * @alist: The #GgnAccountList object that issued the callback.
   *
   * Emitted when the account information is changed by a get/set function
   **/
  signals[MODIFIED] =
    g_signal_new ("modified",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnAccountListClass, modified),
                  NULL, NULL,
                  ggn_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /**
   * GgnAccountList::checked:
   * @alist: The #GgnAccountList object that issued the callback.
   *
   * Emitted when the account status is changed by a check action.
   **/
  signals[CHECKED] =
    g_signal_new ("checked",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnAccountListClass, checked),
                  NULL, NULL,
                  ggn_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /**
   * GgnAccountList::failed:
   * @alist: The #GgnAccountList object that issued the callback.
   *
   * Emitted when the account checking action fails.
   **/
  signals[FAILED] =
    g_signal_new ("failed",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnAccountListClass, failed),
                  NULL, NULL,
                  ggn_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}

/*
 * ggn_account_list_finalize:
 *
 * This function is used by the gobject library to cleanly finish
 * the destruction process started by the dispose function.
 */
static void ggn_account_list_finalize (GObject *obj) {
  /* make a reference to ourself. */
  GgnAccountList *self = GGN_ACCOUNT_LIST (obj);

  /* reduce the reference count on our prefs object. */
  if (self->priv->prefs)
    g_object_unref (G_OBJECT (self->priv->prefs));

  /* free the accounts list. */
  ggn_account_list_accounts_clear (self);
  g_ptr_array_free (self->priv->accounts, TRUE);

  /* destroy the private object. */
  g_free (self->priv);
  self->priv = NULL;

  /* chain up to the parent class. */
  G_OBJECT_CLASS (ggn_account_list_parent_class)->finalize (obj);
}

/**
 * ggn_account_list_new:
 *
 * Creates a new #GgnAccountList with default values, which are
 * used to manage gmail accounts for the notifier.
 *
 * Returns: the new #GgnAccountList object. Free with ggn_account_list_free().
 **/
GgnAccountList *ggn_account_list_new (void) {
  /* make a newly created gobject. */
  GgnAccountList *alist = g_object_new (GGN_TYPE_ACCOUNT_LIST, NULL);

  /* return the new object. */
  return alist;
}

/**
 * ggn_account_list_new_from_prefs:
 * @prefs: The #GgnPrefs object to use.
 *
 * Creates a new #GgnAccountList with values which are pulled from the
 * given #GgnPrefs object, which simplifies the initialization of the
 * account list for outside managing objects.
 *
 * Returns: the new #GgnAccountList object. Free with ggn_account_list_free().
 **/
GgnAccountList *ggn_account_list_new_from_prefs (GgnPrefs *prefs) {
  /* declare a few helping variables. */
  GSList *emails;
  gchar *email;
  guint num;

  /* make a newly created gobject. */
  GgnAccountList *alist = g_object_new (GGN_TYPE_ACCOUNT_LIST, NULL);

  /* ensure the prefs object is non-NULL. */
  if (!prefs)
    return alist;

  /* set the preferences object and increase its reference count. */
  alist->priv->prefs = prefs;
  g_object_ref (prefs);

  /* pull in the list of emails we have. */
  emails = ggn_prefs_get_list (alist->priv->prefs, GGN_PREF_ACCOUNT_LIST);

  /* operate only if we have a list to use. */
  if (emails) {
    /* loop for the number of emails in the list. */
    for (num = 0; num < g_slist_length (emails); num++) {
      /* get the email string. */
      email = (gchar *) g_slist_nth_data (emails, num);

      /* create the given account object. */
      alist->priv->account = ggn_account_new_from_prefs (alist->priv->prefs,
                                                         email);

      /* connect to the "modified" signal. */
      alist->priv->account->priv->handler_modified =
        g_signal_connect (G_OBJECT (alist->priv->account),
                          "modified",
                          G_CALLBACK (ggn_account_list_account_modified),
                          alist);

      /* connect to the "updated" signal. */
      alist->priv->account->priv->handler_updated =
        g_signal_connect (G_OBJECT (alist->priv->account),
                          "updated",
                          G_CALLBACK (ggn_account_list_account_updated),
                          alist);

      /* add the account to the list. */
      g_ptr_array_add (alist->priv->accounts, alist->priv->account);
    }

    /* free the emails list. */
    g_slist_free (emails);
  }

  /* return the new object. */
  return alist;
}

/**
 * ggn_account_list_free:
 * @alist: The #GgnAccountList to free.
 *
 * Frees the given #GgnAccountList object by decreasing its reference count
 * until the #GObject library is forced to free the memory allocated
 * for its use.
 *
 * Returns: void.
 **/
void ggn_account_list_free (GgnAccountList *alist) {
  /* while this object exists... */
  while (G_IS_OBJECT (alist)) {
    /* unreference this object. */
    g_object_unref (G_OBJECT (alist));
  }
}

/**
 * ggn_account_list_length:
 * @alist: The #GgnAccountList to utilize.
 *
 * Returns the number of #GgnAccount objects that the given #GgnAccountList
 * is managing inside its #GPtrArray.
 *
 * Returns: A #guint representing the number of accounts we have.
 **/
guint ggn_account_list_length (GgnAccountList *alist) {
  /* return the length of the pointer array. */
  return alist->priv->accounts->len;
}

/**
 * ggn_account_list_get_num_nonempty:
 * @alist: The #GgnAccountList to utilize.
 *
 * Returns the number of #GgnAccount objects that the given #GgnAccountList
 * is managing inside its #GPtrArray, but only those which contain new or
 * unread entries.
 *
 * Returns: A #guint representing the number of non-empty accounts we have.
 **/
guint ggn_account_list_get_num_nonempty (GgnAccountList *alist) {
  /* return the pre-computed value. */
  return alist->priv->num_nonempty;
}

/**
 * ggn_account_list_get_num_enabled:
 * @alist: The #GgnAccountList to utilize.
 *
 * Returns the number of #GgnAccount objects that the given #GgnAccountList
 * is managing inside its #GPtrArray, but only those which are enabled and
 * not already checking mail.
 *
 * Returns: A #guint representing the number of accounts we have.
 **/
guint ggn_account_list_get_num_enabled (GgnAccountList *alist) {
  /* declare some helping variables. */
  guint idx, num;

  /* initially... */
  num = 0;

  /* loop through the accounts. */
  for (idx = 0; idx < alist->priv->accounts->len; idx++) {
    /* gain a reference to the account. */
    alist->priv->account = (GgnAccount *) 
      g_ptr_array_index (alist->priv->accounts, idx);

    /* is the account enabled? */
    if (ggn_account_get_enabled (alist->priv->account))
      num++;
  }

  /* return the computed value. */
  return num;
}

/**
 * ggn_account_list_get_num_failed:
 * @alist: The #GgnAccountList to utilize.
 *
 * Returns the number of failed #GgnAccount objects that the given
 * #GgnAccountList is managing inside its #GPtrArray.
 *
 * Returns: A #guint representing the number of accounts which have failed.
 **/
guint ggn_account_list_get_num_failed (GgnAccountList *alist) {
  /* return the pre-computed value. */
  return alist->priv->num_failed;
}

/**
 * ggn_account_list_get_num_entries:
 * @alist: The #GgnAccountList to utilize.
 * @n_unread: A pointer to the total unread entry counter.
 * @n_new: A pointer to the total new entry counter.
 *
 * Returns the numbers of entries, both unread and new, which have
 * been summed up over all managed accounts during the check.
 *
 * Returns: void.
 **/
void ggn_account_list_get_num_entries (GgnAccountList *alist,
                                       guint *n_unread,
                                       guint *n_new) {
  /* return the pointed-to values. */
  *n_unread = alist->priv->num_unread_entries;
  *n_new = alist->priv->num_new_entries;
}

/**
 * ggn_account_list_is_checking:
 * @alist: The #GgnAccountList to utilize.
 *
 * Returns whether or not we are currently checking mail by scanning through
 * the accounts we are managing and seeing if any are checking for mail.
 *
 * Returns: A #gboolean value.
 **/
gboolean ggn_account_list_is_checking (GgnAccountList *alist) {
  /* declare a few helping variables. */
  gboolean enab;
  guint state;
  guint idx;

  /* loop through the list of accounts. */
  for (idx = 0; idx < alist->priv->accounts->len; idx++) {
    /* get the account at the current index. */
    alist->priv->account = (GgnAccount *) 
      g_ptr_array_index (alist->priv->accounts, idx);

    /* get the account information required for a decision. */
    enab = ggn_account_get_enabled (alist->priv->account);
    state = ggn_account_get_state (alist->priv->account);

    /* if the account is not checking already... */
    if (state == GGN_ACCOUNT_STATE_CHECKING && enab == TRUE) {
      /* yep. */
      return TRUE;
    }
  }

  /* nope. */
  return FALSE;
}

/**
 * ggn_account_list_search:
 * @alist: The #GgnAccountList to utilize.
 * @email: The email of the account to look for.
 *
 * Returns the account index with the matching email string as given
 * by the calling function, or -1 if no match exists.
 *
 * Returns: A #gint of the account index, or -1.
 **/
gint ggn_account_list_search (GgnAccountList *alist, const gchar *email) {
  /* declare a few helping variables. */
  gchar *compare;
  guint idx;

  /* to avoid warnings. */
  compare = NULL;

  /* loop through the list of accounts. */
  for (idx = 0; idx < alist->priv->accounts->len; idx++) {
    /* get the account at the current index. */
    alist->priv->account = (GgnAccount *) 
      g_ptr_array_index (alist->priv->accounts, idx);

    /* get the comparison email string. */
    compare = ggn_account_get_email (alist->priv->account);

    /* see if this account email matches. */
    if (g_utf8_collate (compare, email) == 0) {
      /* return the account. */
      g_free (compare);
      return idx;
    }
  }

  /* not a one found, return NULL. */
  g_free (compare);
  return -1;
}

/**
 * ggn_account_list_add:
 * @alist: The #GgnAccountList to utilize.
 * @account: The #GgnAccount to add to the managed list.
 *
 * Adds a new account to the list of accounts that are managed by the
 * given account list.
 *
 * Returns: void.
 **/
void ggn_account_list_add (GgnAccountList *alist, GgnAccount *account) {
  /* give the account our preferences object. */
  ggn_account_give_prefs (account, alist->priv->prefs);

  /* link the "modified" callback to our function. */
  account->priv->handler_modified =
    g_signal_connect (G_OBJECT (account),
                      "modified",
                      G_CALLBACK (ggn_account_list_account_modified),
                      alist);

  /* link the "updated" callback to our function. */
  account->priv->handler_updated =
    g_signal_connect (G_OBJECT (account),
                      "updated",
                      G_CALLBACK (ggn_account_list_account_updated),
                      alist);

  /* add the account to the pointer array. */
  g_ptr_array_add (alist->priv->accounts, account);

  /* finally, write the changes to gconf and gnome-keyring. */
  ggn_account_set_prefs (account);
}

/**
 * ggn_account_list_del:
 * @alist: The #GgnAccountList to utilize.
 * @idx: The index of the account to remove from the managed list.
 *
 * Removes a given account at index @idx from the managed list.
 *
 * Returns: void.
 **/
void ggn_account_list_del (GgnAccountList *alist, guint idx) {
  /* check bounds. */
  if (idx >= alist->priv->accounts->len)
    return;

  /* get the account at the given index first. */
  alist->priv->account = g_ptr_array_index (alist->priv->accounts, idx);

  /* remove the account from the pointer array. */
  g_ptr_array_remove_index (alist->priv->accounts, idx);

  /* disconnect the "modified" account signal handler. */
  g_signal_handler_disconnect (G_OBJECT (alist->priv->account),
                               alist->priv->account->priv->handler_modified);

  /* disconnect the "updated" account signal handler. */
  g_signal_handler_disconnect (G_OBJECT (alist->priv->account),
                               alist->priv->account->priv->handler_updated);

  /* unset the account preferences. */
  ggn_account_unset_prefs (alist->priv->account);

  /* free the account object. */
  ggn_account_free (alist->priv->account);
}

/**
 * ggn_account_list_check:
 * @alist: The #GgnAccountList to utilize.
 *
 * Begins the mail-checking process for each account in the managed list
 * and emits a signal when all accounts have completed their checking
 * thread.
 *
 * Returns: void.
 **/
void ggn_account_list_check (GgnAccountList *alist) {
  /* declare a few helping variables. */
  gboolean enab;
  guint state;
  guint idx;

  /* reset the count variables. */
  alist->priv->num_nonempty = 0;
  alist->priv->num_failed = 0;
  alist->priv->num_unread_entries = 0;
  alist->priv->num_new_entries = 0;

  /* loop through the list of accounts. */
  for (idx = 0; idx < alist->priv->accounts->len; idx++) {
    /* get the account at the current index. */
    alist->priv->account = (GgnAccount *) 
      g_ptr_array_index (alist->priv->accounts, idx);

    /* get the account information required for a decision. */
    enab = ggn_account_get_enabled (alist->priv->account);
    state = ggn_account_get_state (alist->priv->account);

    /* if the account is not checking already... */
    if (state != GGN_ACCOUNT_STATE_CHECKING && enab == TRUE) {
      /* run the check routine on the account. */
      ggn_account_check (alist->priv->account);
    }
  }
}

/**
 * ggn_account_list_get:
 * @alist: The #GgnAccountList to utilize.
 * @idx: The index of the account to get.
 *
 * Returns the #GgnAccount at the index @idx in the pointer array.
 *
 * Returns: A #GgnAccount object managed by the account list.
 **/
GgnAccount *ggn_account_list_get (GgnAccountList *alist, guint idx) {
  /* check bounds. */
  if (idx >= alist->priv->accounts->len)
    return NULL;

  /* return the value. */
  return g_ptr_array_index (alist->priv->accounts, idx);
}

/**
 * ggn_account_list_get_default:
 * @alist: The #GgnAccountList to utilize.
 *
 * Returns the default #GgnAccount in the managed account list.
 *
 * Returns: A #GgnAccount object managed by the account list.
 **/
GgnAccount *ggn_account_list_get_default (GgnAccountList *alist) {
  /* declare some helping variables. */
  guint idx;

  /* loop in the list of accounts. */
  for (idx = 0; idx < alist->priv->accounts->len; idx++) {
    /* get the account at the current index. */
    alist->priv->account = (GgnAccount *) 
      g_ptr_array_index (alist->priv->accounts, idx);

    /* if the account is default... */
    if (ggn_account_get_default (alist->priv->account)) {
      /* return this account. */
      return alist->priv->account;
    }
  }

  /* we didn't find an account. */
  return NULL;
}

