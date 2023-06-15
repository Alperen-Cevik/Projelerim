const mongoose = require("mongoose");
const Review = require('./review');

const imageSchema = new mongoose.Schema({
    url: String,
    filename: String,
    _id: false
});

imageSchema.virtual('thumbnail').get(function () {
    return this.url.replace('upload', 'upload/w_200');
});

const campgroundSchema = new mongoose.Schema({
    title: String,
    price: Number,
    description: String,
    location: String,
    geometry: {
        type: {
            type: String,
            enum: ["Point"],
            required: true
        },
        coordinates: {
            type: [Number],
            required: true
        }
    },
    images: [imageSchema],
    owner: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'User'
    },
    reviews: [
        {
            type: mongoose.Schema.Types.ObjectId,
            ref: 'Review'
        }
    ]
}, { toJSON: { virtuals: true } });

campgroundSchema.virtual('properties.popupMarkup').get(function () {
    return `<a href="/campgrounds/${this._id}">${this.title}</a><p>${this.description.substring(0, 20)}...</p>`;
});

campgroundSchema.post('findOneAndDelete', async (campground) => {
    if (campground.reviews.length)
        await Review.deleteMany({ _id: { $in: campground.reviews } });
});

module.exports = mongoose.model("Campground", campgroundSchema);