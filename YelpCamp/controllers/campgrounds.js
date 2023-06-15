const Campground = require("../models/campground");
const mbxClient = require('@mapbox/mapbox-sdk/services/geocoding');
const { cloudinary } = require('../utils/cloudinary');

const baseClient = mbxClient({ accessToken: process.env.MAPBOX_TOKEN });

module.exports.renderIndexPage = async (req, res) => {
    const campgrounds = await Campground.find({});
    res.render('campgrounds/index', { campgrounds });
};

module.exports.getCampgrounds = async (req, res) => {
    const campgrounds = await Campground.find({});
    res.json({ features: campgrounds });
}

module.exports.createCampground = async (req, res) => {
    const geoData = await baseClient.forwardGeocode({
        query: req.body.campground.location,
        limit: 1
    }).send();
    const campground = new Campground(req.body.campground);
    campground.geometry = geoData.body.features[0].geometry;
    campground.owner = req.user._id;
    campground.images = req.files.map(({ path, filename }) => ({ filename, url: path }));
    await campground.save();
    console.log(campground);
    req.flash("alert", { type: "success", message: "Successfully created campground!" });
    res.redirect(`/campgrounds/${campground._id}`);
};

module.exports.updateCampground = async (req, res) => {
    const campground = await Campground.findByIdAndUpdate(req.params.id, {
        ...req.body.campground,
        $push: {
            images: {
                $each: req.files.map(({ filename, path }) => ({ url: path, filename }))
            }
        }
    }, { returnDocument: "after" });
    if (req.body.deleteImages) { // its not set if nothing is checked
        for (let image of req.body.deleteImages) {
            await cloudinary.uploader.destroy(image);
        }
        await campground.updateOne({
            $pull: {
                images: {
                    filename: {
                        $in: req.body.deleteImages
                    }
                }
            }
        });
    }
    req.flash("alert", { type: "info", message: "Successfully updated campground!" });
    res.redirect(`/campgrounds/${campground._id}`);
};

module.exports.deleteCampground = async (req, res) => {
    const deletedCampground = await Campground.findByIdAndDelete(req.params.id);
    res.redirect('/campgrounds');
};

module.exports.renderNewPage = (req, res) => {
    res.render('campgrounds/new');
};

module.exports.renderDetailsPage = async (req, res) => {
    const { id } = req.params;
    const campground = await Campground.findById(id).populate({
        path: 'reviews',
        populate: {
            path: 'author',
            select: 'username'
        }
    }).populate('owner', 'username');
    if (campground)
        return res.render('campgrounds/details', { campground });
    req.flash("alert", { type: "danger", message: "Campground could not be found!" });
    res.redirect('/campgrounds');
};

module.exports.renderEditPage = async (req, res) => {
    const campground = await Campground.findById(req.params.id);
    res.render('campgrounds/edit', { campground });
};