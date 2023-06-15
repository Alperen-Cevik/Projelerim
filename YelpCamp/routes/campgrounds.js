const express = require("express");
const multer = require('multer');

const campgroundController = require("../controllers/campgrounds");
const catchAsync = require("../utils/catchAsync");
const { isAuthenticated, isAuthorized, validateCampground } = require("../utils/middleware");
const { storage } = require('../utils/cloudinary');

const router = express.Router();
const upload = multer({ storage });

router.route('/')
    .get(catchAsync(campgroundController.renderIndexPage))
    .post(isAuthenticated, upload.array('image'), validateCampground, catchAsync(campgroundController.createCampground));

router.get('/geojson', catchAsync(campgroundController.getCampgrounds));

router.get('/new', isAuthenticated, campgroundController.renderNewPage);

router.route('/:id')
    .get(catchAsync(campgroundController.renderDetailsPage))
    .put(isAuthenticated, isAuthorized, upload.array('image'), validateCampground, catchAsync(campgroundController.updateCampground))
    .delete(isAuthenticated, isAuthorized, catchAsync(campgroundController.deleteCampground));

router.get('/:id/edit', isAuthenticated, isAuthorized, catchAsync(campgroundController.renderEditPage));

module.exports = router;