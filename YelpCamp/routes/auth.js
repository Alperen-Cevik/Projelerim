const express = require('express');
const passport = require('passport');

const authController = require('../controllers/auth');

const router = express.Router();

router.route('/register')
    .get(authController.renderRegisterPage)
    .post(authController.register);

router.route('/login')
    .get(authController.renderLoginPage)
    .post(passport.authenticate('local', {
        failureFlash: true,
        failureRedirect: '/login',
        keepSessionInfo: true
    }), authController.login);

router.post('/logout', authController.logout);

module.exports = router;