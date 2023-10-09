import { TestBed } from '@angular/core/testing';

import { KinesisService } from './kinesis.service';

describe('KinesisService', () => {
  let service: KinesisService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(KinesisService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
