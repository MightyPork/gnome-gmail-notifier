/**
 * @file src/ggn-audio.h The GgnAudio object
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

/*
 * ensure this is a once-only header.
 */
#ifndef __GNOME_GMAIL_NOTIFIER_AUDIO_H__
#define __GNOME_GMAIL_NOTIFIER_AUDIO_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * object function macros.
 */
#define GGN_TYPE_AUDIO (ggn_audio_get_type ())
#define GGN_AUDIO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GGN_TYPE_AUDIO, GgnAudio))
#define GGN_AUDIO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GGN_TYPE_AUDIO, GgnAudioClass))
#define GGN_IS_AUDIO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GGN_TYPE_AUDIO))
#define GGN_IS_AUDIO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GGN_TYPE_AUDIO))
#define GGN_AUDIO_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), GGN_TYPE_AUDIO, GgnAudioClass))

/*
 * type macros.
 */
typedef struct _GgnAudio GgnAudio;
typedef struct _GgnAudioClass GgnAudioClass;
typedef struct _GgnAudioPrivate GgnAudioPrivate;

/*
 * object definition.
 */
struct _GgnAudio {
  /* parent gobject class. */
  GObject parent_object;

  /* private instance members. */
  GgnAudioPrivate *priv;
};

/*
 * class definition.
 */
struct _GgnAudioClass {
  /* parent gobject class. */
  GObjectClass parent;
};

/*
 * gobject-type definition.
 */
GType ggn_audio_get_type (void);

/*
 * object creation functions.
 */
GgnAudio *ggn_audio_new (void);
void ggn_audio_free (GgnAudio *audio);

/*
 * sound file playback function.
 */
void ggn_audio_play_file (GgnAudio *audio, const gchar *filename);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_AUDIO_H__ */

