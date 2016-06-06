/**
 * @file src/ggn-audio.c The GgnAudio object
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
 * SECTION:GgnAudio
 * @short_description: The audio player object
 * @include: src/ggn-audio.h
 **/

/*
 * include our application header.
 */
#include <main.h>
#include "ggn-audio-priv.h"

/*
 * forward function definitions.
 */
static void ggn_audio_init (GgnAudio *self);
static void ggn_audio_class_init (GgnAudioClass *klass);
static void ggn_audio_finalize (GObject *obj);

/*
 * define the gobject type and its basic functions.
 */
G_DEFINE_TYPE (GgnAudio, ggn_audio, G_TYPE_OBJECT);

/**
 * ggn_audio_bus_call:
 * @bus: The gstreamer #GstBus that emitted the signal.
 * @msg: The gstreamer #GstMessage that was sent.
 * @data: Local data we gave to the calling object.
 *
 * Called when the gstreamer message bus sends information about
 * the currently active (playing) audio stream.
 *
 * Returns: callback success gboolean.
 **/
static gboolean ggn_audio_bus_call (GstBus *bus,
                                    GstMessage *msg,
                                    gpointer data) {
  /* regain our structure from the data. */
  GgnAudio *audio = GGN_AUDIO (data);

  /* determine the type of message that was sent. */
  switch (GST_MESSAGE_TYPE (msg)) {
    /* end of stream. */
    case GST_MESSAGE_EOS: {
      /* end iteration of the audio loop. */
      gst_element_set_state (audio->priv->pipeline, GST_STATE_NULL);
      g_main_loop_quit (audio->priv->loop);

      /* leave the switch */
      break;
    }

    /* error message. */
    case GST_MESSAGE_ERROR: {
      /* end iteration of the audio loop. */
      gst_element_set_state (audio->priv->pipeline, GST_STATE_NULL);
      g_main_loop_quit (audio->priv->loop);

      /* leave the switch */
      break;
    }

    /* anything else. */
    default:
      break;
  }

  /* return success. */
  return TRUE;
}

/**
 * ggn_audio_pad_added:
 * @element: The gstreamer #GstElement that emitted the signal.
 * @pad: The gstreamer #GstPad that was added to the element.
 * @data: Local data we gave to the calling object.
 *
 * Becuase elements in our pipeline utilize dynamic pads, a callback
 * must be set up to handle their initialization in a special way.
 * This function finalizes the linkage in the pipeline.
 *
 * Returns: void
 **/
static void ggn_audio_pad_added (GstElement *element,
                                 GstPad *pad,
                                 gpointer data) {
  /* pull our audioconvert element out of the data. */
  GstElement *convert = (GstElement *) data;

  /* define the sink pad. */
  GstPad *sinkpad;

  /* get the dynamic pad for the end of the chain. */
  sinkpad = gst_element_get_pad (convert, "sink");

  /* link the two pads together. */
  gst_pad_link (pad, sinkpad);

  /* unreference our sinkpad, as it's no longer needed. */
  gst_object_unref (sinkpad);
}

/*
 * ggn_audio_init:
 *
 * This function is used by the gobject library to
 * generate a new instance of our object.
 */
static void ggn_audio_init (GgnAudio *self) {
  /* set up the private data structure. */
  self->priv = g_new0 (GgnAudioPrivate, 1);

  /* setup the audio loop. */
  self->priv->loop = g_main_loop_new (NULL, FALSE);

  /* create the gstreamer pipeline. */
  self->priv->pipeline = gst_pipeline_new (PACKAGE);

  /* create the filesrc element. */
  self->priv->source = gst_element_factory_make ("filesrc", "source");

  /* create the wavparse element. */
  self->priv->parse = gst_element_factory_make ("wavparse", "parse");

  /* create the audioconvert element. */
  self->priv->convert = gst_element_factory_make ("audioconvert", "convert");

  /* create the audioresample element. */
  self->priv->sample = gst_element_factory_make ("audioresample", "sample");

  /* create the autoaudiosink element. */
  self->priv->sink = gst_element_factory_make ("autoaudiosink", "sink");

  /* add a bus watch for important messages. */
  GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (self->priv->pipeline));
  gst_bus_add_watch (bus, ggn_audio_bus_call, self);
  gst_object_unref (bus);

  /* add all the elements into the pipeline. */
  gst_bin_add_many (GST_BIN (self->priv->pipeline),
                    self->priv->source, self->priv->parse,
                    self->priv->convert, self->priv->sample,
                    self->priv->sink, NULL);

  /* link the source and sink ends together. */
  gst_element_link (self->priv->source, self->priv->parse);
  gst_element_link_many (self->priv->convert, self->priv->sample,
                         self->priv->sink, NULL);

  /* use a callback to link the two chains. */
  g_signal_connect (G_OBJECT (self->priv->parse),
                    "pad-added",
                    G_CALLBACK (ggn_audio_pad_added),
                    self->priv->convert);
}

/*
 * ggn_audio_class_init:
 *
 * This function is used by the gobject library to
 * generate a new class object of our object.
 */
static void ggn_audio_class_init (GgnAudioClass *klass) {
  /* setup a gobject class. */
  GObjectClass *gobj_class = G_OBJECT_CLASS (klass);

  /* set the locations of our destruction function. */
  gobj_class->finalize = ggn_audio_finalize;
}

/*
 * ggn_audio_finalize:
 *
 * This function is used by the gobject library to cleanly finish
 * the destruction process started by the dispose function.
 */
static void ggn_audio_finalize (GObject *obj) {
  /* make a reference to ourself. */
  GgnAudio *self = GGN_AUDIO (obj);

  /* ensure that the audio loop is stopped. */
  g_main_loop_quit (self->priv->loop);

  /* free the private instance objects. */
  g_main_loop_unref (self->priv->loop);
  gst_object_unref (self->priv->pipeline);

  /* destroy the private object. */
  g_free (self->priv);
  self->priv = NULL;

  /* chain up to the parent class. */
  G_OBJECT_CLASS (ggn_audio_parent_class)->finalize (obj);
}

/**
 * ggn_audio_new:
 *
 * Creates a new #GgnAudio with default values, which are
 * used to play audio files for the notifier.
 *
 * Returns: the new #GgnAudio object. Free with ggn_audio_free().
 **/
GgnAudio *ggn_audio_new (void) {
  /* make a newly created gobject. */
  GgnAudio *audio = g_object_new (GGN_TYPE_AUDIO, NULL);

  /* return the new object. */
  return audio;
}

/**
 * ggn_audio_free:
 * @audio: The #GgnAudio to free.
 *
 * Frees the given #GgnAudio object by decreasing its reference count
 * until the #GObject library is forced to free the memory allocated
 * for its use.
 *
 * Returns: void.
 **/
void ggn_audio_free (GgnAudio *audio) {
  /* while this object exists... */
  while (G_IS_OBJECT (audio)) {
    /* unreference this object. */
    g_object_unref (G_OBJECT (audio));
  }
}

/**
 * ggn_audio_play_file_thread:
 * @data: Private data passed to the thread.
 *
 * A function used by the ggn_audio_play_file() function in order to
 * play all audio in a separate thread from the main application.
 *
 * Returns: thread private data.
 **/
gpointer ggn_audio_play_file_thread (gpointer data) {
  /* get our audio object. */
  GgnAudio *audio = GGN_AUDIO (data);

  /* set the pipeline to playing and kickoff the audio loop. */
  gst_element_set_state (audio->priv->pipeline, GST_STATE_PLAYING);
  g_main_loop_run (audio->priv->loop);

  /* quit the thread. */
  return data;
}

/**
 * ggn_audio_play_file:
 * @audio: The #GgnAudio to play the file.
 * @filename: The filename string of the file to play.
 *
 * As long as the pipeline we have set up in the given #GgnAudio
 * object is not currently playing a file, this function plays
 * @filename through the default audio output sink.
 *
 * Returns: void.
 **/
void ggn_audio_play_file (GgnAudio *audio, const gchar *filename) {
  /* make sure the filename is not NULL. */
  if (!filename) {
    /* exit the function. */
    return;
  }

  /* make sure the file exists. */
  if (!g_file_test (filename, G_FILE_TEST_IS_REGULAR)) {
    /* exit the function. */
    return;
  }

  /* set the location argument for the filesrc element. */
  g_object_set (G_OBJECT (audio->priv->source),
                "location", filename, NULL);

  /* spawn a new thread to handle playback. */
  g_thread_create (ggn_audio_play_file_thread,
                   (gpointer) audio,
                   FALSE, NULL);
}

