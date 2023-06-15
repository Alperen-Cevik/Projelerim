const BaseJoi = require('joi');
const sanitizeHtml = require('sanitize-html');

const extension = (joi) => ({
    type: 'object',
    base: joi.object(),
    messages: {
        'object.escapeHTML': '{{#label}} must not include HTML!'
    },
    rules: {
        escapeHTML: {
            validate(value, helpers) {
                let jsonString = JSON.stringify(value);
                const clean = sanitizeHtml(jsonString, {
                    allowedTags: [],
                    allowedAttributes: {},
                });
                if (clean !== jsonString)
                    return helpers.error('object.escapeHTML', { jsonString })
                return clean;
            }
        }
    }
});

const Joi = BaseJoi.extend(extension)

module.exports.campgroundValidator = Joi.object({
    campground: Joi.object({
        title: Joi.string().required(),
        price: Joi.number().min(0).required(),
        // image: Joi.string().required(),
        description: Joi.string().required(),
        location: Joi.string().required(),
    }).required().escapeHTML(),
    deleteImages: Joi.array()
});

module.exports.reviewValidator = Joi.object({
    review: Joi.object({
        body: Joi.string().required(),
        rating: Joi.number().min(1).max(5).required(),
    }).required().escapeHTML()
});