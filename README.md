# GStreamer Plugins CV

A set of plugins with the purpose of giving a simple and adaptive set of plugins for application developers of computer vision applications.

## Elements
### Detectors
#### gstcvdlibobjectdetecthog
This is a filter that allows to detect an object (cars, faces, etc) given a model.
Example:
```
gst-launch-1.0 v4l2src ! videoconvert ! cvdlibobjectdetecthog model-location=model.dat draw=1 ! videoconvert ! xvimagesink
```
*Note: it is based on the HOG detector from dlib. Dlib embeds a model to detect faces on its source code, so there is not such as a public model from Dlib. You can try it by writing a small program to deserialize the embeded model into a file, training your own model following Dlib's instrusctions or looking for models on the web like [this](https://github.com/danishnazir/Car-Detection-using-HOG-based-Detector-Dlib-/blob/master/car_detector.svm).*

### Image processing
#### gstcvscale
This is a filter to scale down (or up) a frame. This element is intended to be used in combination with other filters supporting it. Example:
```
gst-launch-1.0 v4l2src ! videoconvert ! cvscale scale-factor=0.25 cvdlibobjectdetecthog model-location=model.dat draw=1 ! videoconvert ! xvimagesink
```

## Build flatpak
There is a flatpak plugin just for testing purposes. Use as this:
```
./tools/build-flatpak.sh
flatpak install io.github.cfoch.gst_plugins_cv.json
```
