/*
    src/checkbox.cpp -- Two-state check box widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/checkbox.h>
#include <nanovg.h>
#include <nanogui/theme.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(CheckBox, Widget)

CheckBox::CheckBox(Widget *parent, const std::string &caption,
                   const std::function<void(bool) > &callback)
    : Widget(parent), mCaption(caption), mPushed(false), mChecked(false),
      mCallback(callback) {

    mIconExtraScale = 1.2f;// widget override
}

bool CheckBox::mouseButtonEvent(const Vector2i &p, int button, bool down,
                                int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);
    if (!mEnabled)
        return false;

    if (isMouseButtonLeft(button)) {
        if (down) {
            mPushed = true;
        } else if (mPushed) {
            if (contains(p)) {
                mChecked = !mChecked;
                if (mCallback)
                    mCallback(mChecked);
            }
            mPushed = false;
        }
        return true;
    }
    return false;
}

Vector2i CheckBox::preferredSize(NVGcontext *ctx) const {
    nvgFontSize(ctx, (float)fontSize());
    nvgFontFace(ctx, "sans");
    Vector2i prefSize( nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr) + 1.8f * fontSize(),
                       fontSize() * 1.3f);

    if (mFixedSize.x() > 0)
      prefSize.x() = mFixedSize.x();
    if (mFixedSize.y() > 0)
      prefSize.y() = mFixedSize.y();

    return prefSize;
}

void CheckBox::draw(NVGcontext *ctx) {
    Widget::draw(ctx);

    nvgFontSize(ctx, (float)fontSize());
    nvgFontFace(ctx, "sans");
    nvgFillColor(ctx,
                 mEnabled ? mTheme->mTextColor : mTheme->mDisabledTextColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(ctx, mPos.x() + 1.6f * fontSize(), mPos.y() + mSize.y() * 0.5f,
            mCaption.c_str(), nullptr);

    const Color& pushedColor = mPushedColor.w() ? mPushedColor : mTheme->mCheckboxPushedColor;
    const Color& bgColor = mChecked
                              ? (mCheckedColor.w() ? mCheckedColor : mTheme->mCheckboxCheckedColor)
                              : (mUncheckedColor.w() ? mUncheckedColor : mTheme->mCheckboxUncheckedColor);

    NVGpaint bg = nvgBoxGradient(ctx, mPos.x() + 1.5f, mPos.y() + 1.5f,
                                 mSize.y() - 2.0f, mSize.y() - 2.0f, 3, 3,
                                 mPushed ? pushedColor : bgColor,
                                 Color(0, 0, 0, 180));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + 1.0f, mPos.y() + 1.0f,
                        mSize.y() - 2.0f, mSize.y() - 2.0f, 3);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    if (mChecked) {
        nvgFontSize(ctx, mSize.y() * icon_scale());
        nvgFontFace(ctx, "icons");
        nvgFillColor(ctx, mEnabled ? mTheme->mIconColor
                                   : mTheme->mDisabledTextColor);
        nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgText(ctx, mPos.x() + mSize.y() * 0.5f + 1,
                mPos.y() + mSize.y() * 0.5f, utf8(mTheme->mCheckBoxIcon).data(),
                nullptr);
    }
}

void CheckBox::save(Serializer &s) const {
    Widget::save(s);
    s.set("caption", mCaption);
    s.set("pushed", mPushed);
    s.set("checked", mChecked);
}

bool CheckBox::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("caption", mCaption)) return false;
    if (!s.get("pushed", mPushed)) return false;
    if (!s.get("checked", mChecked)) return false;
    return true;
}

NAMESPACE_END(nanogui)
