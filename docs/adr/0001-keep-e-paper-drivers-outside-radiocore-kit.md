# Keep e-paper panel drivers outside RadioCore_Kit

RadioCore_Kit owns RadioCore board configuration and board-adaptive examples,
while reusable e-paper panel drivers remain in `heltec-eink-modules`. This keeps
the e-paper library independently usable and avoids coupling panel-controller
behavior to RadioCore hardware, at the cost of coordinating compatible releases
across the two repositories.
