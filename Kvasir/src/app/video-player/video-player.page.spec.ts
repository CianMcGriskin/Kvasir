import { ComponentFixture, TestBed } from '@angular/core/testing';
import { VideoPlayerPage } from './video-player.page';

describe('VideoPlayerPage', () => {
  let component: VideoPlayerPage;
  let fixture: ComponentFixture<VideoPlayerPage>;

  beforeEach(async(() => {
    fixture = TestBed.createComponent(VideoPlayerPage);
    component = fixture.componentInstance;
    fixture.detectChanges();
  }));

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
