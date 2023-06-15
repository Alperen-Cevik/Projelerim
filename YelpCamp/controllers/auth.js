const User = require('../models/user');

module.exports.renderRegisterPage = (req, res) => {
    res.render('auth/register');
};

module.exports.register = async (req, res, next) => {
    try {
        const { username, password, email } = req.body;
        const user = await User.register(new User({ email, username }), password);
        req.login(user, e => {
            if (e)
                return next(e);
            req.flash('alert', { type: 'success', message: 'Welcome to YelpCamp!' });
            res.redirect('/campgrounds');
        });
    }
    catch (e) {
        req.flash('alert', { type: 'danger', message: e.message });
        res.redirect('/register');
    }
};

module.exports.renderLoginPage = (req, res) => {
    res.render('auth/login');
};

module.exports.login = (req, res) => {
    req.flash('alert', { type: 'success', message: 'Welcome back!' });
    let redirectUrl = req.session.returnTo || '/campgrounds';
    delete req.session.returnTo;
    res.redirect(redirectUrl);
};

module.exports.logout = (req, res, next) => {
    req.logout((err) => {
        if (err)
            return next(err);
        req.flash('alert', { type: 'success', message: 'Goodbye!' });
        res.redirect('/campgrounds');
    });
};