const mongoose = require("mongoose");

mongoose.connect('mongodb://localhost:27017/yelp-camp', {
    useNewUrlParser: true,
    useUnifiedTopology: true,
}).then(() => {
    console.log("Connection is open!");
}).catch(err => {
    console.log(err);
});

const Campground = require('../models/campground');
const cities = require('./cities');
const helper = require('./seedHelpers');

const pickSample = (array) => array[Math.floor(Math.random() * array.length)];

const seedDb = async () => {
    await Campground.deleteMany({});
    for (let i = 0; i < 300; i++) {
        const { city, state, longitude, latitude } = pickSample(cities);
        const campground = new Campground({
            location: `${city}, ${state}`,
            title: `${pickSample(helper.descriptors)} ${pickSample(helper.places)}`,
            price: Math.floor(Math.random() * 20) + 10,
            geometry: {
                type: "Point",
                coordinates: [longitude, latitude]
            },
            owner: "62aee9ec8bb78f9efd764f01",
            description: "Lorem ipsum dolor sit amet consectetur adipisicing elit. Incidunt asperiores modi debitis quibusdam nulla, nostrum fugit dolorem ut enim culpa velit, molestiae aperiam rerum officiis delectus voluptatem beatae expedita ab?",
            images: [{ url: "https://source.unsplash.com/1600x900?in-the-woods", filename: "nothing" }],
            reviews: []
        });
        await campground.save();
    }
};

seedDb().then(() => {
    mongoose.connection.close();
});