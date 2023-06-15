const ExpressError = require('./ExpressError');
const Campground = require("../models/campground");
const Review = require('../models/review');
const { campgroundValidator, reviewValidator } = require('./joiValidators');

module.exports.isAuthenticated = (req, res, next) => {
    if (req.isAuthenticated())
        return next();
    req.session.returnTo = req.originalUrl;
    req.flash('alert', { type: 'danger', message: 'You must be logged in to view that page!' });
    res.redirect('/login');
}

module.exports.isAuthorized = async (req, res, next) => {
    const { id } = req.params;
    const campground = await Campground.findById(id);
    if (campground.owner.equals(req.user._id))
        return next();
    req.flash('alert', { type: 'danger', message: 'You are not authorized for this action!' });
    res.redirect(`/campgrounds/${campground._id}`);
}

module.exports.doesReviewAuthorMatch = async (req, res, next) => {
    const { id, reviewId } = req.params;
    const review = await Review.findById(reviewId);
    if (review.author.equals(req.user._id))
        return next();
    req.flash('alert', { type: 'danger', message: 'You are not authorized for this action.' });
    res.redirect(`/campgrounds/${id}`);
}

module.exports.validateCampground = (req, res, next) => {
    const { error } = campgroundValidator.validate(req.body);
    if (error)
        throw new ExpressError(400, error.details.map(d => d.message).join(', '));
    next();
}

module.exports.validateReview = (req, res, next) => {
    const { error } = reviewValidator.validate(req.body);
    if (error)
        throw new ExpressError(400, error.details.map(d => d.message).join(', '));
    next();
}