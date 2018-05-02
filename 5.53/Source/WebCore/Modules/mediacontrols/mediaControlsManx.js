function createControls(root, video, host)
{
    return new ControllerManx(root, video, host);
};

function ControllerManx(root, video, host)
{
    this.video = video;
    this.root = root;
    this.host = host;
    this.controls = {};
    this.listeners = {};

    this.addVideoListeners();
    this.createBase();
    this.createControls();

    this.updateBase();
    this.updateControls();
    this.updateDuration();
    this.updatePlaying();
};

ControllerManx.prototype = {

    HandledVideoEvents: {
        durationchange: 'handleDurationChange',
        pause: 'handlePause',
        playing: 'handlePlay',
    },
    ClassNames: {
        hidden: 'hidden',
        paused: 'paused',
        show: 'show',
    },

    listenFor: function(element, eventName, handler, useCapture)
    {
        if (typeof useCapture === 'undefined')
            useCapture = false;

        if (!(this.listeners[eventName] instanceof Array))
            this.listeners[eventName] = [];

        this.listeners[eventName].push({element:element, handler:handler, useCapture:useCapture});
        element.addEventListener(eventName, this, useCapture);
    },

    addVideoListeners: function()
    {
        for (var name in this.HandledVideoEvents) {
            this.listenFor(this.video, name, this.HandledVideoEvents[name]);
        };

        this.controlsObserver = new MutationObserver(this.handleControlsChange.bind(this));
        this.controlsObserver.observe(this.video, { attributes: true, attributeFilter: ['controls'] });
    },

    handleEvent: function(event)
    {
        var preventDefault = false;

        try {
            if (event.target === this.video) {
                var handlerName = this.HandledVideoEvents[event.type];
                var handler = this[handlerName];
                if (handler && handler instanceof Function)
                    handler.call(this, event);
            }

            if (!(this.listeners[event.type] instanceof Array))
                return;

            this.listeners[event.type].forEach(function(entry) {
                if (entry.element === event.currentTarget && entry.handler instanceof Function)
                    preventDefault |= entry.handler.call(this, event);
            }, this);
        } catch(e) {
            if (window.console)
                console.error(e);
        }

        if (preventDefault) {
            event.stopPropagation();
            event.preventDefault();
        }
    },

    createBase: function()
    {
        var base = this.base = document.createElement('div');
        base.setAttribute('pseudo', '-webkit-media-controls');
        this.listenFor(base, 'mousemove', this.handleWrapperMouseMove);
        this.listenFor(base, 'mouseout', this.handleWrapperMouseOut);
        if (this.host.textTrackContainer)
            base.appendChild(this.host.textTrackContainer);
    },

    createControls: function()
    {
        var panelCompositedParent = this.controls.panelCompositedParent = document.createElement('div');
        panelCompositedParent.setAttribute('pseudo', '-webkit-media-controls-panel-composited-parent');

        var panel = this.controls.panel = document.createElement('div');
        panel.setAttribute('pseudo', '-webkit-media-controls-panel');
        panel.setAttribute('role', 'toolbar');
        this.listenFor(panel, 'transitionend', this.handlePanelTransitionEnd);
        this.listenFor(panel, 'click', this.handlePanelClick);

        var playButton = this.controls.playButton = document.createElement('button');
        playButton.setAttribute('pseudo', '-webkit-media-controls-play-button');
        this.listenFor(playButton, 'click', this.handlePlayButtonClicked);

        var rewindButton = this.controls.rewindButton = document.createElement('button');
        rewindButton.setAttribute('pseudo', '-webkit-media-controls-rewind-button');
        this.listenFor(rewindButton, 'click', this.handleRewindButtonClicked);

        var fullscreenButton = this.controls.fullscreenButton = document.createElement('button');
        fullscreenButton.setAttribute('pseudo', '-webkit-media-controls-fullscreen-button');
        this.listenFor(fullscreenButton, 'click', this.handleFullscreenButtonClicked);
    },

    updateBase: function()
    {
        if (this.shouldHaveControls()) {
            if (!this.base.parentNode) {
                this.root.appendChild(this.base);
            }
        } else {
            if (this.base.parentNode) {
                this.base.parentNode.removeChild(this.base);
            }
        }
    },

    updateControls: function()
    {
        this.disconnectControls();
        this.configureControls();

        if (this.shouldHaveControls())
            this.addControls();
    },

    disconnectControls: function(event)
    {
        for (var item in this.controls) {
            var control = this.controls[item];
            if (control && control.parentNode)
                control.parentNode.removeChild(control);
       }
    },

    configureControls: function()
    {
        this.controls.panel.appendChild(this.controls.playButton);
        this.controls.panel.appendChild(this.controls.rewindButton);
        if (!this.isAudio())
            this.controls.panel.appendChild(this.controls.fullscreenButton);
    },

    handleDurationChange: function(event)
    {
        this.updateDuration();
    },

    handlePause: function(event)
    {
        this.setPlaying(false);
    },

    handlePlay: function(event)
    {
        this.setPlaying(true);
    },

    handleWrapperMouseMove: function(event)
    {
        this.showControls();
    },

    handleWrapperMouseOut: function(event)
    {
        this.hideControls();
    },

    handlePanelTransitionEnd: function(event)
    {
        var opacity = window.getComputedStyle(this.controls.panel).opacity;
        if (parseInt(opacity) > 0)
            this.controls.panel.classList.remove(this.ClassNames.hidden);
        else
            this.controls.panel.classList.add(this.ClassNames.hidden);
    },

    handlePanelClick: function(event)
    {
        // Prevent clicks in the panel from playing or pausing the video in a MediaDocument.
        event.preventDefault();
    },

    handlePlayButtonClicked: function(event)
    {
        if (this.canPlay())
            this.video.play();
        else
            this.video.pause();
        return true;
    },

    handleRewindButtonClicked: function(event)
    {
        this.video.pause();
        this.video.currentTime = this.video.seekable.start(0);
        return true;
    },

    handleFullscreenButtonClicked: function(event)
    {
        this.video.webkitEnterFullscreen();
    },

    handleControlsChange: function()
    {
        try {
            this.updateBase();
            if (this.shouldHaveControls())
                this.addControls();
            else
                this.removeControls();
        } catch(e) {
            if (window.console)
                console.error(e);
        }
    },

    updateDuration: function()
    {
        if (this.video.duration === Number.POSITIVE_INFINITY)
            this.controls.rewindButton.classList.add(this.ClassNames.hidden)
        else
            this.controls.rewindButton.classList.remove(this.ClassNames.hidden)
    },

    updatePlaying: function()
    {
        this.setPlaying(!this.video.paused);
    },

    setPlaying: function(isPlaying)
    {
        if (this.isPlaying === isPlaying)
            return;
        this.isPlaying = isPlaying;

        if (!isPlaying) {
            this.controls.panel.classList.add(this.ClassNames.paused);
            this.controls.playButton.classList.add(this.ClassNames.paused);
        } else {
            this.controls.panel.classList.remove(this.ClassNames.paused);
            this.controls.playButton.classList.remove(this.ClassNames.paused);
            this.hideControls();
        }
    },

    addControls: function()
    {
        this.base.appendChild(this.controls.panelCompositedParent);
        this.controls.panelCompositedParent.appendChild(this.controls.panel);
    },

    removeControls: function()
    {
        if (this.controls.panel.parentNode)
            this.controls.panel.parentNode.removeChild(this.controls.panel);
    },

    showControls: function()
    {
        this.controls.panel.classList.add(this.ClassNames.show);
        this.controls.panel.classList.remove(this.ClassNames.hidden);
    },

    hideControls: function()
    {
        this.controls.panel.classList.remove(this.ClassNames.show);
    },

    canPlay: function()
    {
        return this.video.paused || this.video.ended;
    },

    isAudio: function()
    {
        return this.video instanceof HTMLAudioElement;
    },

    shouldHaveControls: function()
    {
        return this.video.controls;
    },
};
