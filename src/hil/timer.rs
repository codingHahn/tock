use core::cell::Cell;
use alarm::{Alarm, AlarmClient};

pub trait Timer {
    fn now(&self) -> u32;
    //fn cancel(&mut self, request: TimerRequest);
    fn oneshot(&self, interval: u32);
    fn repeat(&self, interval: u32);
}

pub trait TimerClient {
    fn fired(&self, now: u32);
}

pub struct SingleTimer<'a, Alrm: Alarm + 'a> {
    interval: Cell<u32>,
    when: Cell<u32>,
    repeat: Cell<bool>,
    alarm: &'a Alrm
}

impl<'a, Alrm: Alarm> SingleTimer<'a, Alrm> {
    pub fn new(alarm: &'a Alrm) -> SingleTimer<'a, Alrm> {
        SingleTimer {
            interval: Cell::new(0),
            when: Cell::new(0),
            repeat: Cell::new(false),
            alarm: alarm
        }
    }
}

impl<'a, Alrm: Alarm> Timer for SingleTimer<'a, Alrm> {
    fn now(&self) -> u32 {
        self.alarm.now()
    }

    fn oneshot(&self, interval: u32) {
        let when = interval.wrapping_add(self.alarm.now());

        self.when.set(when);

        self.interval.set(interval);
        self.repeat.set(false);

        self.alarm.set_alarm(when);
    }

    fn repeat(&self, interval: u32) {
        let when = interval.wrapping_add(self.alarm.now());

        self.when.set(when);

        self.interval.set(interval);
        self.repeat.set(true);

        self.alarm.set_alarm(when);
    }
}

impl<'a, Alrm: Alarm> AlarmClient for SingleTimer<'a, Alrm> {
    fn fired(&self) {
        let repeat = self.repeat.get();
        if repeat {
            let interval = self.interval.get();
            let when = interval.wrapping_add(self.now());

            self.when.set(when);

            self.alarm.set_alarm(when);
        } else {
            self.alarm.disable_alarm();
        }
    }
}

