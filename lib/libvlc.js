export class MediaPlayer {
  constructor(module, path) {
    this.module = module;
    //this.media_player_ptr = module._libvlc_media_player_new_from_path(path);
    this.media_player_ptr = module._get_media_player_buck_bunny();
  }

  toggle_play() {
    if (!this.is_playing()) {
      this.play();
    }
    else {
      this.pause();
    }
  }


  play() {
    return this.module._play(this.media_player_ptr);
  }

  pause() {
    return this.module._pause_video(this.media_player_ptr);
  }

  is_playing() {
    return this.module._is_playing(this.media_player_ptr);
  }

  get_position() {
    return this.module._get_position(this.media_player_ptr);
  }

  set_position(position, fast = 0) {
    // TODO - what does "fast" argument do?
    return this.module._set_position(this.media_player_ptr, position, fast);
  }

  get_volume() {
    return this.module._get_volume(this.media_player_ptr);
  }

  set_volume(volume) {
    return this.module._set_volume(this.media_player_ptr, volume);
  }

  toggle_mute() {
    return this.module._toggle_mute(this.media_player_ptr);
  }

  get_mute() {
    return this.module._get_mute(this.media_player_ptr);
  }

  set_mute(mute) {
    return this.module._set_mute(this.media_player_ptr, mute);
  }
}
