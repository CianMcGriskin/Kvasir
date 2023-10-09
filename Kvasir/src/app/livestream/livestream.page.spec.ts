import { ComponentFixture, TestBed } from '@angular/core/testing';
import { LivestreamPage } from './livestream.page';

describe('LivestreamPage', () => {
  let component: LivestreamPage;
  let fixture: ComponentFixture<LivestreamPage>;

  beforeEach(async(() => {
    fixture = TestBed.createComponent(LivestreamPage);
    component = fixture.componentInstance;
    fixture.detectChanges();
  }));

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
