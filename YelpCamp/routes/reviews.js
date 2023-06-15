const express = require("express");

const reviewController = require("../controllers/reviews");
const catchAsync = require("../utils/catchAsync");
const { isAuthenticated, doesReviewAuthorMatch, validateReview } = require("../utils/middleware");

const router = express.Router({ mergeParams: true });

router.post('/', isAuthenticated, validateReview, catchAsync(reviewController.createReview));
router.delete('/:reviewId', isAuthenticated, doesReviewAuthorMatch, catchAsync(reviewController.deleteReview));

module.exports = router;