# GStreamer Plugins CV

A set of plugins with the purpose of giving a simple and adaptive set of plugins for application developers of computer vision applications.

## Build flatpak
The easiest way to try this project is using flatpak.

First, install the GNOME SDK:
```
    flatpak install org.gnome.Sdk//3.32
```

and then build and install this project:
```
./tools/build-flatpak.sh
flatpak install io.github.cfoch.gst_plugins_cv.flatpak
```

## Usage from flatpak
If you have built a flatpak, start a bash interpreter in the sandbox with:

```
flatpak run --command=bash io.github.cfoch.gst_plugins_cv
```

and then run the commands below, the ones starting with `gst-launch-1.0 ...`.


## Elements
### Object detectors
#### gstcvdlibobjectdetecthog
This is a filter that allows to detect an object (cars, faces, etc) given a model.
Example:
```
gst-launch-1.0 v4l2src ! videoconvert ! cvdlibobjectdetecthog model-location=model.dat draw=1 ! videoconvert ! xvimagesink
```
*Note: it is based on the HOG detector from dlib. Dlib embeds a model to detect faces on its source code, so there is not such as a public model from Dlib that you can easily download. You can try it by writing a small program to deserialize the embeded model into a file, training your own model following Dlib's instrusctions or looking for models on the web like [this](https://github.com/danishnazir/Car-Detection-using-HOG-based-Detector-Dlib-/blob/master/car_detector.svm).*

### Image processing
#### gstcvscale
This is a filter to scale down (or up) a frame. This element is intended to be used in combination with other filters supporting it. Example:
```
gst-launch-1.0 v4l2src ! videoconvert ! cvscale scale-factor=0.25 ! cvdlibobjectdetecthog model-location=model.dat draw=1 ! videoconvert ! xvimagesink
```

#### cvdraw
This is a filter to draw object info received in the buffer meta.

Draw everything:
```
gst-launch-1.0 -v v4l2src ! videoconvert ! \
    cvdlibobjectdetecthog model-location=model.dat sub-key=cv,label=face ! cvdraw ! \
    videoconvert ! xvimagesink
```

Draw everything with sub key `label=face` and `element=detect`:
```
gst-launch-1.0 -v v4l2src ! videoconvert ! \
    cvdlibobjectdetecthog name=detect model-location=model.dat sub-key=cv,label=face,element=detect ! \
    cvdraw sub-key=cv,label=face,element=detect ! \
    videoconvert ! xvimagesink
```

Or you can automatically add `element=detect` or whatever the peer (previous) element is:
```
gst-launch-1.0 -v v4l2src ! videoconvert ! \
    cvdlibobjectdetecthog name=detect model-location=model.dat sub-key=cv,label=face ! \
    cvdraw on-peer=true sub-key=cv,label=face ! \
    videoconvert ! xvimagesink
```
*However, if `element=foo` is set explicitly on the `sub-key` property, it will be overriden with the peer.*

You can also draw whatever is on the params of object info (only index param supported for now):
```
gst-launch-1.0 -v v4l2src ! videoconvert ! \
    cvdlibobjectdetecthog name=detect model-location=../facedetection-hog.svm sub-key=cv,label=face ! \
    cvdraw on-peer=true sub-key=cv,label=face params="cv,params=<index,other_param>" ! \
    videoconvert ! xvimagesink
```
