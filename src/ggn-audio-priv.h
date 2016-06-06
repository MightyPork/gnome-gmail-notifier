/**
 * @file src/ggn-audio-priv.h The GgnAudio object
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
 * GgnAudioPrivate:
 * @loop: The #GMainLoop used for playback.
 * @pipeline: The gstreamer playback pipeline.
 * @source: The wav file source element.
 * @parse: The parsing element.
 * @convert: The wav->pcm conversion element.
 * @sample: The resampling element.
 * @sink: The audio output sink.
 *
 * The #GgnAudio is a simple GObject wrapper for playing WAV
 * files using the gstreamer audio manipulation library.
 **/
struct _GgnAudioPrivate {
  /* the audio playback loop. */
  GMainLoop *loop;

  /* the playback pipeline. */
  GstElement *pipeline;

  /* the pipeline elements. */
  GstElement *source;
  GstElement *parse;
  GstElement *convert;
  GstElement *sample;
  GstElement *sink;
};

