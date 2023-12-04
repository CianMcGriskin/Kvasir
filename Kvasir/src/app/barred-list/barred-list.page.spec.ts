import { ComponentFixture, TestBed } from '@angular/core/testing';
import { BarredListPage } from './barred-list.page';

describe('BarredListPage', () => {
  let component: BarredListPage;
  let fixture: ComponentFixture<BarredListPage>;

  beforeEach(async(() => {
    fixture = TestBed.createComponent(BarredListPage);
    component = fixture.componentInstance;
    fixture.detectChanges();
  }));

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
