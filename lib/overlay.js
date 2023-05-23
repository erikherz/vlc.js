// Functions to manage both the layout and user inputs of the video overlay
// It's very artisanal and would usually be done with a framework like
// React, but it's enough for the needs of the project.

const PROGRESS_BAR_COLOR = "#2a81d4";
const PROGRESS_BAR_BG_COLOR = "#2d2d2d";
const VOLUME_BAR_COLOR = "#4ad24a";
const VOLUME_BAR_MUTE_COLOR = "#4d704e";
const VOLUME_BAR_BG_COLOR = "#2d2d2d";

const BUTTON_BG_COLOR = "#464646";
const GENERAL_BG_COLOR = "#353535";
const TEXT_COLOR = "#8d8d8c";

const MENU_BAR_HEIGHT = 30;
const BUTTON_SIZE = 20;
const GAP_SIZE = 5;

function get_layout(canvas) {
  const VOLUME_BAR_WIDTH = 100;
  const VOLUME_BAR_HEIGHT = 20;

  const PROGRESS_TEXT_WIDTH = 80;

  const PROGRESS_BAR_WIDTH = canvas.width -
  (GAP_SIZE + BUTTON_SIZE + GAP_SIZE + GAP_SIZE + BUTTON_SIZE + GAP_SIZE + PROGRESS_TEXT_WIDTH + GAP_SIZE + VOLUME_BAR_WIDTH + GAP_SIZE);
  const PROGRESS_BAR_HEIGHT = 10;

  const PLAY_BUTTON_X = GAP_SIZE;
  const PROGRESS_BAR_X = PLAY_BUTTON_X + BUTTON_SIZE + GAP_SIZE;
  const PROGRESS_TEXT_X = PROGRESS_BAR_X + PROGRESS_BAR_WIDTH + GAP_SIZE;
  const VOLUME_BUTTON_X = PROGRESS_TEXT_X + PROGRESS_TEXT_WIDTH + GAP_SIZE;
  const VOLUME_BAR_X = VOLUME_BUTTON_X + BUTTON_SIZE + GAP_SIZE;

  return {
    VOLUME_BAR_WIDTH,
    VOLUME_BAR_HEIGHT,
    PROGRESS_TEXT_WIDTH,
    PROGRESS_BAR_WIDTH,
    PROGRESS_BAR_HEIGHT,
    PLAY_BUTTON_X,
    PROGRESS_BAR_X,
    PROGRESS_TEXT_X,
    VOLUME_BUTTON_X,
    VOLUME_BAR_X,
  };
}

const central_play_icon = new Image(50, 50);
central_play_icon.src = "vlc/modules/gui/qt/pixmaps/icons/play.svg";

const play_button = new Image(BUTTON_SIZE, BUTTON_SIZE);
play_button.src = "vlc/modules/gui/qt/pixmaps/icons/play.svg";
const pause_button = new Image(BUTTON_SIZE, BUTTON_SIZE);
pause_button.src = "vlc/modules/gui/qt/pixmaps/icons/pause.svg";

const volume_button = new Image(BUTTON_SIZE, BUTTON_SIZE);
volume_button.src = "vlc/modules/gui/qt/pixmaps/icons/volume_medium.svg";
const muted_button = new Image(BUTTON_SIZE, BUTTON_SIZE);
muted_button.src = "vlc/modules/gui/qt/pixmaps/icons/volume_muted.svg";

// Apply underlying changes (eg video is paused) to the displayed UI
export function update_overlay(overlay) {
  const media_player = window.media_player;
  if (media_player == null) {
    return;
  }

  let is_paused = !media_player.is_playing();

  const playButton = document.getElementById('play-button');
  const bPlayButton = document.getElementById('bottom-play-button');
  const timer = document.getElementById('time');
  const volumeSvg = document.getElementById('volume-svg');
  const progressCurrent = document.getElementById('bottom-progress-value');
  const progressCurrentVolume = document.getElementById('bottom-progress-volume-value');

  let progressPosition = media_player.get_position() * 100;
  if (progressPosition > 100) {
    progressPosition = 100;
  }
  progressCurrent.style.setProperty('--current-video-progress-width', `${progressPosition}%`);

  // Set volume
  const isMuted = media_player.get_mute();

  progressCurrentVolume.style.setProperty('--current-audio-progress-width', `${isMuted ? 0 : media_player.get_volume()}%`);

  if (isMuted) {
    volumeSvg.innerHTML = `<path d="M12.4 12.5l2.1-2.1 2.1 2.1 1.4-1.4L15.9 9 18 6.9l-1.4-1.4-2.1 2.1-2.1-2.1L11 6.9 13.1 9 11 11.1zM3.786 6.008H.714C.286 6.008 0 6.31 0 6.76v4.512c0 .452.286.752.714.752h3.072l4.071 3.858c.5.3 1.143 0 1.143-.602V2.752c0-.601-.643-.977-1.143-.601L3.786 6.008z"></path>`;
  } else {
    volumeSvg.innerHTML = `<path d="M15.6 3.3c-.4-.4-1-.4-1.4 0-.4.4-.4 1 0 1.4C15.4 5.9 16 7.4 16 9c0 1.6-.6 3.1-1.8 4.3-.4.4-.4 1 0 1.4.2.2.5.3.7.3.3 0 .5-.1.7-.3C17.1 13.2 18 11.2 18 9s-.9-4.2-2.4-5.7z"></path><path d="M11.282 5.282a.909.909 0 000 1.316c.735.735.995 1.458.995 2.402 0 .936-.425 1.917-.995 2.487a.909.909 0 000 1.316c.145.145.636.262 1.018.156a.725.725 0 00.298-.156C13.773 11.733 14.13 10.16 14.13 9c0-.17-.002-.34-.011-.51-.053-.992-.319-2.005-1.522-3.208a.909.909 0 00-1.316 0zm-7.496.726H.714C.286 6.008 0 6.31 0 6.76v4.512c0 .452.286.752.714.752h3.072l4.071 3.858c.5.3 1.143 0 1.143-.602V2.752c0-.601-.643-.977-1.143-.601L3.786 6.008z"></path>`;
  }

  // Set duration text
  let time = media_player.get_time();
  let seconds = Math.trunc(time / 1000);
  let minutes = Math.trunc(seconds / 60);
  seconds = ('00'+seconds % 60).slice(-2);
  let max_time = media_player.get_length();
  let max_seconds = Math.trunc(max_time / 1000);
  let max_minutes = Math.trunc(max_seconds / 60);
  max_seconds = ('00' + (max_seconds % 60)).slice(-2);

  timer.innerHTML = `${minutes}:${seconds} / ${max_minutes}:${max_seconds}`;

  if (is_paused && globalThis.files) {
    playButton.innerHTML = `<div><svg><path d="M15.562 8.1L3.87.225c-.818-.562-1.87 0-1.87.9v15.75c0 .9 1.052 1.462 1.87.9L15.563 9.9c.584-.45.584-1.35 0-1.8z"></path></svg></div>`;
    bPlayButton.innerHTML = `<svg class="bottom-bar-play-button-svg"><path d="M15.562 8.1L3.87.225c-.818-.562-1.87 0-1.87.9v15.75c0 .9 1.052 1.462 1.87.9L15.563 9.9c.584-.45.584-1.35 0-1.8z"></path></svg>`
  }

  if (!is_paused && globalThis.files) {
    playButton.innerHTML = ``;
    bPlayButton.innerHTML = `<svg class="bottom-bar-play-button-svg"><path d="M6 1H3c-.6 0-1 .4-1 1v14c0 .6.4 1 1 1h3c.6 0 1-.4 1-1V2c0-.6-.4-1-1-1zm6 0c-.6 0-1 .4-1 1v14c0 .6.4 1 1 1h3c.6 0 1-.4 1-1V2c0-.6-.4-1-1-1h-3z"></path></svg>`;
  }

  const nextChapter = document.getElementById('next-chapter');
  const previousChapter = document.getElementById('previous-chapter');

  const currentChapter = media_player.get_chapter();
  const chapterCount = media_player.get_chapter_count();
  if (chapterCount) {
    if (currentChapter > 0) {
      previousChapter.style.visibility = 'visible';
    } else {
      previousChapter.style.visibility = 'hidden';
    }
  
    if (currentChapter < chapterCount) {
      nextChapter.style.visibility = 'visible';
    } else {
      nextChapter.style.visibility = 'hidden';
    }
  }
}

export function on_overlay_click(overlay, mouse_event) {

}
