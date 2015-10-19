# Mural (Experimental)

**Currently under heavy development, a way much better alpha version is coming soon :D**

The project is an experiment which implements [CanvasRenderingContext2D][CanvasRenderingContext2D]
using amazing [Cinder][Cinder] framework and [duktape][duktape].

It's designed to be something like __Ejecta for Desktop__.
The ONLY reason that I start from scratch instead of using [NW.js][NW.js] is
its exported file size. Pack a simple game into a browser and
send it to friends sounds crazy and not everyone wants to download
another __Chromium__ just for playing a tiny game.

The core part is based on [Ejecta][Ejecta], with some extra candy
from [Cinder][Cinder]. The most important goal of this project is to wrap HTML5 games,
but the C++ layer is designed to be clean and independent from any JavaScript VMs
which means you can also use it as a **C++ game engine**. Since [Cinder][Cinder] has
GREAT power, I'll recommend you to try it yourself.

## Features

### CanvasRenderingContext2D API

- [x] strokeStyle, fillStyle (color)
- [x] fillStyle (pattern)
- [ ] strokeStyle, fillStyle (gradient)
- [x] globalAlpha
- [x] globalCompositeOperation
- [x] save, restore
- [x] beginPath, closePath
- [x] moveTo, lineTo
- [x] rect, arc
- [x] stroke, fill
- [x] strokeRect, fillRect
- [ ] strokeText, fillText
- [x] drawImage
- [x] translate, rotate, scale
- [x] clearRect
- [x] bezierCurveTo
- [x] arcTo
- [x] quadraticCurveTo
- [x] setTrasform
- [x] transform
- [x] createImageData (Surface API is great, no need to create ourselves)
- [x] getImageData
- [x] putImageData
- [x] lineWidth
- [x] lineCap
- [x] lineJoin
- [x] miterLimit
- [ ] font
- [ ] texAlign
- [ ] textBaseline
- [ ] measureText
- [ ] imageSmoothingEnabled
- [ ] shadowBlur
- [ ] shadowColor
- [ ] shadowOffsetX
- [ ] shadowOffsetY
- [ ] createLinearGradient
- [ ] createRadialGradient
- [ ] createPattern

### Timers

- [x] setTimeout, setInterval, clearTimeout, clearInterval
- [x] requestAnimationFrame

## Progress of developing

I always prefer [Trello][trello] for project management, the tasks list can be found here [Mural at Trello][mural-trello]

## Build and Run

1. Download [Cinder][Cinder] latest version (0.9.0) or clone its `master` branch
2. Create an `apps` folder inside your cinder folder
3. Clone Mural into the `apps` folder, so you'll have `apps/Mural`
4. Now open the project file, and have fun

(You can also put Mural to any path and set the `CINDER_PATH` config from the project file)

## Supported Platforms

Windows and OSX are on the top priority while Linux,
iOS and Android supports are planned (as soon as the `master` branch and
Cinder support for Linux/Android are stable enough).

---

The MIT License (MIT)

Copyright (c) 2014-2015 Sean Bohan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

[CanvasRenderingContext2D]: https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D
[Cinder]: https://libcinder.org/
[duktape]: http://duktape.org/
[Ejecta]: http://impactjs.com/ejecta
[Cocoonjs]: https://www.ludei.com/cocoonjs/
[NW.js]: http://nwjs.io/
[trello]: https://trello.com
[mural-trello]: https://trello.com/b/9cpESvdR/mural
