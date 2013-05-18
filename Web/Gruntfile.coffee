module.exports = (grunt) ->
  'use strict'

  grunt.initConfig
    coffee:
      debug:
        files: [
          expand: true
          cwd: 'js'
          src: ['**/*.coffee', '!**/_*.coffee']
          dest: 'public/js'
          ext: '.js'
        ]

    stylus:
      debug:
        files: [
          expand: true
          cwd: 'css'
          src: ['**/*.styl', '!**/_*.styl']
          dest: 'public/css'
          ext: '.css'
        ]

    watch:
      coffee:
        files: ['assets/js/**/*.coffee']
        tasks: ['coffee']

      stylus:
        files: ['assets/css/**/*.styl']
        tasks: ['stylus']

  grunt.loadNpmTasks 'grunt-contrib-coffee'
  grunt.loadNpmTasks 'grunt-contrib-stylus'

  grunt.registerTask 'debug', ['coffee:debug', 'stylus:debug']
  grunt.registerTask 'default', ['debug']
